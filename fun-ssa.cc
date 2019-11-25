#include <iostream>
// fun-ssa.cc -- IR function SSA form
//
// Copyright © 2019  Miles Bader
//
// Author: Miles Bader <snogglethorpe@gmail.com>
// Created: 2019-11-25
//

#include <set>
#include <map>

#include "check-assertion.h"

#include "reg.h"
#include "insn.h"
#include "phi-fun-insn.h"
#include "phi-fun-inp-insn.h"

#include "fun.h"


// Insert SSA phi-functions in every place they're needed in this
// function.
//
void
Fun::insert_phi_functions ()
{
  for (auto bb : _blocks)
    {
      std::set<Reg *> bb_regs;
      for (auto insn : bb->insns ())
	for (auto reg : insn->results ())
	  bb_regs.insert (reg);

      for (auto frontier_block : bb->dominance_frontier ())
	for (auto reg : bb_regs)
	  {
	    // Search existing phi functions in FRONTIER_BLOCK to see
	    // if there's already one present for REG.
	    //
	    bool found_phi_function = false;
	    for (auto insn : frontier_block->insns ())
	      if (PhiFunInsn *phi_fun = dynamic_cast<PhiFunInsn *> (insn))
		{
		  if (phi_fun->results ()[0] == reg)
		    {
		      // We found a matching phi function!
		      found_phi_function = true;
		      break;
		    }
		}
	      else
		{
		  // Phi-functions occur at the beginning of the
		  // block, so when we run into a non-phi-function
		  // insn, we can just stop looking.

		  break;
		}

	    if (! found_phi_function)
	      new PhiFunInsn (reg, frontier_block);
	  }
    }
}

class HierarchialRegMap
{
public:
  HierarchialRegMap (HierarchialRegMap *parent_map)
    : _parent_map (parent_map)
  { }

  void add (Reg *from, Reg *to)
  {
    _mapping[from] = to;
  }

  Reg *map (Reg *from) const
  {
    auto entry = _mapping.find (from);
    if (entry != _mapping.end ())
      return entry->second;
    else if (_parent_map)
      return _parent_map->map (from);
    else
      return 0;
  }

private:

  std::map<Reg *, Reg *> _mapping;

  HierarchialRegMap *_parent_map = 0;
};


// For each place registers are used / defined 
// replace the registers with the appropriate SSA value registers.
//
// After completion, all registers in this function will have a single
// definition.
//
void
convert_dominated_regs_to_ssa_values (BB *block, HierarchialRegMap *dominating_reg_map = 0)
{
  HierarchialRegMap reg_map (dominating_reg_map);

  for (auto insn : block->insns ())
    {
      // Replace argument registers in INSN with the corresponding
      // SSA values.
      //
      const std::vector<Reg *> &args = insn->args ();
      unsigned num_args = args.size ();
      for (unsigned arg_num = 0; arg_num < num_args; arg_num++)
	{
	  Reg *old_arg = args[arg_num];
	  Reg *new_arg = reg_map.map (old_arg);
	  if (new_arg && new_arg != old_arg)
	    insn->change_arg (arg_num, new_arg);
	}

      // Insert new mappings to reflect INSN's results.
      //
      const std::vector<Reg *> &results = insn->results ();
      unsigned num_results = results.size ();
      for (unsigned result_num = 0; result_num < num_results; result_num++)
	{
	  Reg *old_result = results[result_num];
	  Reg *new_result = old_result->make_ssa_value ();
	  insn->change_result (result_num, new_result);
	  reg_map.add (old_result, new_result);
	}
    }

  // Now process dominated blocks.
  //
  for (auto dominatee : block->dominatees ())
    convert_dominated_regs_to_ssa_values (dominatee, &reg_map);

  // For any successor edges which are on block's dominator frontier,
  // fill in phi-function arguments.
  //
  for (auto succ : block->successors ())
    for (auto insn : succ->insns ())
      if (PhiFunInsn *phi_fun = dynamic_cast<PhiFunInsn *> (insn))
	{
	  Reg *arg_proto = phi_fun->results ()[0]->ssa_proto ();
	  Reg *arg_value = reg_map.map (arg_proto);

	  check_assertion
	    (arg_value,
	     "Phi-function input has no value in predecessor block");

	  new PhiFunInpInsn (phi_fun, arg_value, block);
	}
      else
	break;
}

// Convert this function into SSA form.
//
void
Fun::convert_to_ssa_form ()
{
  insert_phi_functions ();

  convert_dominated_regs_to_ssa_values (_entry_block);
}
