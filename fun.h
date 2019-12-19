// fun.h -- IR functions
//
// Copyright © 2019  Miles Bader
//
// Author: Miles Bader <snogglethorpe@gmail.com>
// Created: 2019-10-28
//

#ifndef __FUN_H__
#define __FUN_H__


#include "bb.h"


class Reg;
class Value;


// IR function, containing a flow graph, a description of resources
// used by the function, and the function's interface.
//
class Fun
{
public:

  Fun ();
  ~Fun ();


  // Add new block to this function.
  //
  void add_block (BB *block)
  {
    block->set_num (++_max_block_num);
    _blocks.push_back (block);
  }

  // Remove BLOCK from this function.
  //
  void remove_block (BB *block);

  // Return a reference to a read-only list containing the blocks in
  // this function.
  //
  const std::list<BB *> &blocks () const { return _blocks; }


  // Return the entry block for this function, which cannot have any
  // predecessors.
  //
  BB *entry_block () const { return _entry_block; }

  // Set the entry block for this function, which cannot have any
  // predecessors.
  //
  void set_entry_block (BB *block) { _entry_block = block; }


  // Return the exit block for this function, which cannot have any
  // successors.
  //
  BB *exit_block () const { return _exit_block; }

  // Set the exit block for this function, which cannot have any
  // successors.
  //
  void set_exit_block (BB *block) { _exit_block = block; }


  // Return a reference to a read-only list containing the registers
  // in this function.
  //
  const std::list<Reg *> &regs () const { return _regs; }

  // Add REG to this function.  Subsquently, the function now owns
  // it, and is responsible for deallocating it.  This does not modify
  // REG to refer to this function.
  //
  void add_reg (Reg *reg) { _regs.push_back (reg); }

  // Remove REG from this function.  It is not deallocated, merely
  // forgotten.  This does not modify REG's reference to this function.
  //
  void remove_reg (Reg *reg) { _regs.remove (reg); }


  // Return a reference to a read-only list containing the values
  // in this function.
  //
  const std::list<Value *> &values () const { return _values; }

  // Add VALUE to this function.  Subsquently, the function now owns
  // it, and is responsible for deallocating it.  This does not modify
  // VALUE to refer to this function.
  //
  void add_value (Value *value) { _values.push_back (value); }

  // Remove VALUE from this function.  It is not deallocated, merely
  // forgotten.  This does not modify VALUE's reference to this function.
  //
  void remove_value (Value *value) { _values.remove (value); }


  // Make sure dominator information in this function is valid.
  //
  void update_dominators ()
  {
    if (! _dominators_valid)
      calc_dominators ();
  }

  // Make sure post-dominator information in this function is valid.
  //
  void update_post_dominators ()
  {
    if (! _post_dominators_valid)
      calc_post_dominators ();
  }


  // Return true if dominator / post-dominator information in this
  // function is up to date.
  //
  bool dominators_valid () const { return _dominators_valid; }
  bool post_dominators_valid () const { return _post_dominators_valid; }


  // Mark dominator / post-dominator information in this
  // function is as out of date.
  //
  void invalidate_dominators () { _dominators_valid = false; }
  void invalidate_post_dominators () { _post_dominators_valid = false; }


  // Convert this function into SSA form.
  //
  void convert_to_ssa_form ();

  // Remove SSA phi-functions from this function, replacing them with
  // equivalent simple copy insns.
  //
  void convert_from_ssa_form ();


  // Remove any unreachable blocks.
  //
  void remove_unreachable ();

  // Try to simplify the flow graph by combining blocks and removing
  // branch insns where possible.
  //
  void combine_blocks ();

  // Where possible, replace uses of registers defined in a copy
  // instruction with the register they're copied from.
  //
  void propagate_through_copies ();

  // Remove all copy instructions whose results are unused.
  //
  void remove_useless_copies ();


private:

  // Calculate the forward dominator tree for all blocks in BLOCKS.
  //
  void calc_dominators () { BB::calc_dominators (_blocks); }

  // Calculate the post dominator tree for all blocks in BLOCKS.
  //
  void calc_post_dominators () { BB::calc_post_dominators (_blocks); }


  // Insert SSA phi-functions in every place they're needed in this
  // function.
  //
  void insert_phi_functions ();


  // List of basic blocks in this function, in no particular order.
  //
  std::list<BB *> _blocks;

  // Entry block for this function.  Cannot have any predecessors.
  //
  BB *_entry_block = 0;

  // Exit block for this function.  Cannot have any successors.
  //
  BB *_exit_block = 0;

  // Registers used in this function.
  //
  std::list<Reg *> _regs;

  // Values used in this function.
  //
  std::list<Value *> _values;

  // Maximum block number used in this function so far.
  //
  unsigned _max_block_num = 0;

  // True if dominator / post-dominator information in this function is up to date.
  //
  bool _dominators_valid = false;
  bool _post_dominators_valid = false;
};


#endif // __FUN_H__
