// fun.cc -- IR functions
//
// Copyright © 2019  Miles Bader
//
// Author: Miles Bader <snogglethorpe@gmail.com>
// Created: 2019-11-02
//

#include <deque>

#include "check-assertion.h"

#include "insn.h"
#include "reg.h"

#include "fun.h"


Fun::Fun ()
{
  // We initialize _ENTRY_BLOCK / _EXIT_BLOCK here to ensure the rest
  // of our state is already initialized, as the BB constructor will
  // add the block to this function..

  _entry_block = new BB (this);
  _exit_block = new BB (this);
}


Fun::~Fun ()
{
  while (! _blocks.empty ())
    delete _blocks.front ();

  while (! _regs.empty ())
    delete _regs.front ();
}


// Remove BLOCK from this function.
//
void
Fun::remove_block (BB *block)
{
  _blocks.remove (block);

  if (_entry_block == block)
    _entry_block = 0;
  if (_exit_block == block)
    _exit_block = 0;
}


// Remove any unreachable blocks.
//
void
Fun::remove_unreachable ()
{
  std::deque<BB *> queue;
  for (auto bb : _blocks)
    if (bb != _entry_block && bb->predecessors ().empty ())
      queue.push_back (bb);

  while (! queue.empty ())
    {
      BB *bb = queue.front ();
      queue.pop_front ();

      //
      // Use a funny loop (instead of "for (... : ...)") because we'd
      // like to remove elements while we're iterator over them.  So
      // we increment the list pointer before possibly removing what
      // it used to point to.
      //

      const std::list<BB *> &succs = bb->successors ();

      auto succp = succs.begin ();
      while (succp != succs.end ())
	{
	  BB *succ = *succp;
	  ++succp;

	  bb->remove_successor (succ);

	  if (succ->predecessors ().empty ())
	    queue.push_back (succ);
	}

      delete bb;
    }
}


// Return true if all entries in the block list BLOCK refer to a
// single block.
//
static bool
singular_block_list (const std::list<BB *> &block_list)
{
  BB *first_block = block_list.front ();

  for (auto block : block_list)
    if (block != first_block)
      return false;

  return true;
}


// Try to simplify the flow graph by combining blocks and removing
// branch insns where possible.
//
void
Fun::combine_blocks ()
{

  check_assertion (_exit_block && _exit_block->successors ().empty (),
		   "Exit block has successors before combine_blocks");
  check_assertion (_exit_block != _entry_block && ! _entry_block->successors ().empty (),
		   "Entry block has no successors before combine_blocks");

  std::deque<BB *> queue;

  bool change = true;
  while (change)
    {
      change = false;

      for (auto bb : _blocks)
	{
	  if (bb == _entry_block || bb == _exit_block)
	    check_assertion (bb->insns ().empty (), "Exit block not empty");

	  // Ignore exit block.
	  //
	  if (bb == _exit_block)
	    continue;

	  // Remove pointless branch insn
	  //
	  const std::list<Insn *> &insns = bb->insns();
	  if (! insns.empty ())
	    {
	      Insn *last_insn = insns.back ();
	      if (last_insn->is_branch_insn ())
		{
		  if (singular_block_list (bb->successors ()))
		    {
		      delete last_insn;
		      change = true;
		    }
		}
	    }

	  // If BB is the only predecessor to its only successor, move
	  // insns from the successor to BB, and set BB's successor to
	  // the successor's successor.
	  //
	  if (bb != _entry_block && bb->successors ().size () == 1)
	    {
	      BB *succ = bb->fall_through ();

	      if (succ != bb && succ != _exit_block && succ->predecessors ().size () == 1)
		{
		  while (! succ->insns ().empty ())
		    bb->add_insn (succ->insns ().front ());

		  bb->set_fall_through (succ->fall_through ());

		  // Orphan SUCC, to simplify the flow graph.
		  //
		  succ->set_fall_through (0);

		  change = true;
		}
	    }
	  else
	    {
	      // Try to skip empty blocks for each successor.

	      for (auto succ : bb->successors ())
		if (succ
		    && succ != _exit_block
		    && succ != bb
		    && succ->insns ().empty ())
		  {
		    BB *new_succ = succ->fall_through ();

		    if (new_succ != succ)
		      {
			if (succ->predecessors ().size () == 1)
			  // Orphan SUCC, to simplify the flow graph.
			  //
			  succ->set_fall_through (0);

			bb->change_successor (succ, new_succ);

			change = true;

			// Upon success, we stop immediately, as the
			// state of our iteration over BB's successors
			// is now uncertain.  We'll try again because
			// CHANGE is now true.
			//
			break;
		      }
		  }
	    }
	}
    }

  check_assertion (_exit_block && _exit_block->successors ().empty (),
		   "Exit block has successors after combine_blocks");
  check_assertion (_exit_block != _entry_block && ! _entry_block->successors ().empty (),
		   "Entry block has no successors after combine_blocks");
}
