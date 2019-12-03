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
#include "copy-insn.h"
#include "phi-fun-insn.h"
#include "phi-fun-inp-insn.h"

#include "fun.h"



// ----------------------------------------------------------------
// Convert to SSA-form
//


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



// ----------------------------------------------------------------
// Convert from SSA-form
//


// Interpose new blocks for each successor to INP_BLOCK which has a
// phi-function input in this block, and move the corresponding
// phi-function input instructions into these new blocks.
//
void
isolate_phi_function_inputs (BB *inp_block)
{
  check_assertion (inp_block->successors ().size () > 1,
		   "Multiple successors expected in isolate_phi_function_inputs");

  // A mapping from phi-function blocks to the new interposing blocks
  // we create.
  //
  std::map<BB *, BB *> interposing_blocks;

  const std::list<Insn *> &inp_block_insns = inp_block->insns ();
  while (! inp_block_insns.empty ())
    {
      auto insn_iter = inp_block_insns.end ();

      // Skip the branch insn at the end (we expect a
      // branch insn because the block has multiple
      // successors).
      //
      --insn_iter;
      check_assertion ((*insn_iter)->is_branch_insn (),
		       "Branch expected at end of block");

      // Now see if the insn before that is a
      // phi-function input.
      //
      --insn_iter;
      if (insn_iter == inp_block_insns.begin ())
	break;

      if (PhiFunInpInsn *inp_to_move
	  = dynamic_cast<PhiFunInpInsn *> (*insn_iter))
	{
	  // Move INP_TO_MOVE.

	  // Block for the corresponding phi-function.
	  //
	  BB *phi_fun_block = inp_to_move->phi_fun ()->block ();

	  // See if there's a new interposing block to move
	  // INP_TO_MOVE to, otherwise create one.
	  //
	  BB *&interposing_block = interposing_blocks[phi_fun_block];
	  if (! interposing_block)
	    {
	      interposing_block = new BB (inp_block->fun ());
	      interposing_block->set_fall_through (phi_fun_block);
	      inp_block->change_successor (phi_fun_block, interposing_block);

	      const std::list<BB *> &succs = inp_block->successors ();
	      check_assertion (std::find (succs.begin(), succs.end(), phi_fun_block) == succs.end(), "@2");
	    }

	  // Move INP_TO_MOVE to the interposing block (which removes
	  // it from INP_BLOCK).
	  //
	  interposing_block->add_insn (inp_to_move);

	  check_assertion (inp_to_move->block () == interposing_block, "@1");
	  const std::list<Insn *> &insns = inp_block->insns ();
	  check_assertion (std::find (insns.begin(), insns.end(), inp_to_move) == insns.end (), "@3");
	}
      else
	{
	  // We've run out of phi-function inputs, so stop the loop.

	  break;
	}
    }
}


// Remove SSA phi-functions from this function, replacing them with
// equivalent simple copy insns.
//
void
Fun::convert_from_ssa_form ()
{
  for (auto bb : _blocks)
    {
      const std::list<Insn *> &insns = bb->insns ();

      while (! insns.empty ())
	{
	  Insn *insn = insns.front ();
	  PhiFunInsn *phi_fun = dynamic_cast<PhiFunInsn *> (insn);

	  if (! phi_fun)
	    break;

	  for (auto inp : phi_fun->inputs ())
	    {
	      BB *inp_block = inp->block ();

	      // If this block has multiple successors, we need to add
	      // special blocks to hold the phi-function inputs, as
	      // they'll be replaced by copy instructions.
	      //
	      // When we do this for one phi-function input, we move
	      // all other phi-function inputs as well; the new blocks
	      // we add will all have a single successor, so
	      // subsequent checks on any of the moved phi-function
	      // inputs will not cause any further blocks to be added.
	      //
	      if (inp_block->successors ().size () > 1)
		{
		  isolate_phi_function_inputs (inp_block);

		  // INP's block will have changed, so re-fetch it.
		  //
		  inp_block = inp->block ();
		}

	      // Add a copy insn to replace the phi-function input.
	      //
	      new CopyInsn (inp->args ()[0], phi_fun->results ()[0],
			    inp_block);
	    }

	  // Remove all the phi-function's inputs.
	  //
	  const std::list<PhiFunInpInsn *> &inputs = phi_fun->inputs ();
	  while (! inputs.empty ())
	    delete inputs.front ();

	  // Finally, get rid of the phi-function.
	  //
	  delete phi_fun;
	}
    }
}
