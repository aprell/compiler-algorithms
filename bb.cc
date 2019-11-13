// bb.cc -- IR basic blocks
//
// Copyright © 2019  Miles Bader
//
// Author: Miles Bader <snogglethorpe@gmail.com>
// Created: 2019-10-28
//

#include "check-assertion.h"

#include "bb.h"
#include "insn.h"

#include "fun.h"


// Make a new block in function FUN.
//
BB::BB (Fun *fun)
  : _fun (fun)
{
  fun->add_block (this);
}


// Destructor; tries to cleanly remove the block from the flow
// graph.
//
BB::~BB ()
{
  while (! _insns.empty ())
    remove_insn (_insns.front ());

  BB *fall_through = _fall_through;

  set_fall_through (0);

  //
  // Use a funny loop (instead of "for (... : ...)") because we'd like
  // to remove elements while we're iterating over the list containing
  // them.  So we increment the list pointer before possibly removing
  // what it used to point to.
  //

  auto predp = _preds.begin ();
  while (predp != _preds.end ())
    {
      BB *pred = *predp;
      ++predp;

      if (pred->fall_through () == this)
	{
	  pred->set_fall_through (fall_through);
	}
      else
	{
	  check_assertion (! pred->_insns.empty (), "No branch insn");

	  Insn *branch_insn = pred->_insns.back ();
	  check_assertion (branch_insn->is_branch_insn (), "No branch insn");

	  branch_insn->change_branch_target (this, fall_through);
	}
    }

  check_assertion (_preds.empty (), "Destroyed block has remaining predecessors");
  check_assertion (_succs.empty (), "Destroyed block has remaining successors");

  Fun *fun = _fun;
  if (fun)
    {
      _fun = 0;
      fun->remove_block (this);
    }
}


// Add the instruction INSN to the end of this block.
//
void
BB::add_insn (Insn *insn)
{
  BB *old_block = insn->block ();

  if (old_block)
    old_block->remove_insn (insn);

  _insns.push_back (insn);

  insn->set_block (this);
}

// Remove the instruction INSN from this block.
//
void
BB::remove_insn (Insn *insn)
{
  _insns.remove (insn);
  insn->set_block (0);
}


// Set the block to which control-flow in this block goes if
// execution runs off the end of it to BLOCK.  BLOCK may be zero.
//
void
BB::set_fall_through (BB *block)
{
  if (block != _fall_through)
    {
      if (_fall_through)
	remove_successor (_fall_through);

      if (block)
	add_successor (block);

      _fall_through = block;
    }
}


// Add a new control flow edge reflecting that SUCC is now a
// successor of this block.
///
void
BB::add_successor (BB *succ)
{
  _succs.push_back (succ);
  succ->_preds.push_back (this);
}

// Remove a control flow edge between this block and the previous
// successor block SUCC.
///
void
BB::remove_successor (BB *succ)
{
  _succs.remove (succ);
  succ->_preds.remove (this);
}
