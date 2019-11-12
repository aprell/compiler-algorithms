// fun.h -- IR functions
//
// Copyright © 2019  Miles Bader
//
// Author: Miles Bader <snogglethorpe@gmail.com>
// Created: 2019-11-02
//

#ifndef __FUN_H__
#define __FUN_H__

#include <ios>

#include "bb.h"


class Reg;


// IR function, containing a flow graph, a description of resources
// used by the function, and the function's interface.
//
class Fun
{
public:

  // Calculate the forward dominator tree for all blocks in BLOCKS.
  //
  void calc_dominators () { BB::calc_dominators (_blocks); }

  // Calculate the reverse dominator tree for all blocks in BLOCKS.
  //
  void calc_reverse_dominators () { BB::calc_post_dominators (_blocks); }


  // Add new block to this function.
  //
  void add_block (BB *block)
  {
    block->set_num (++_max_block_num);
    _blocks.push_back (block);
  }

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


  // Add REG to this function.  Subsquently, the function now owns
  // it, and is responsible for deallocating it.
  //
  void add_reg (Reg *reg) { _regs.push_back (reg); }

  // Return a reference to a read-only list containing the registers
  // in this function.
  //
  const std::list<Reg *> &regs () const { return _regs; }

  // For any blocks which have no predecessors, remove their outgoing
  // edges.
  //
  void propagate_unreachable ();


private:

  // Entry block for this function.  Cannot have any predecessors.
  //
  BB *_entry_block = 0;

  // Exit block for this function.  Cannot have any successors.
  //
  BB *_exit_block = 0;

  // List of basic blocks in this function, in no particular order.
  //
  std::list<BB *> _blocks;

  // Registers used in this function.
  //
  std::list<Reg *> _regs;

  // Maximum block number used in this function so far.
  //
  unsigned _max_block_num = 0;
};


#endif // __FUN_H__
