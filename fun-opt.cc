// fun-opt.cc -- IR function optimizations
//
// Copyright © 2019  Miles Bader
//
// Author: Miles Bader <snogglethorpe@gmail.com>
// Created: 2019-11-02
//

#include <deque>

#include "check-assertion.h"

#include "reg.h"
#include "insn.h"
#include "copy-insn.h"

#include "fun.h"


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
      // like to remove elements while we're iterating over them.  So
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

  check_assertion (!_exit_block || _exit_block->successors ().empty (),
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

  check_assertion (!_exit_block || _exit_block->successors ().empty (),
		   "Exit block has successors after combine_blocks");
  check_assertion (_exit_block != _entry_block && ! _entry_block->successors ().empty (),
		   "Entry block has no successors after combine_blocks");
}


// Where possible, replace uses of registers defined in a copy
// instruction with the register they're copied from.
//
void
Fun::propagate_through_copies ()
{
  for (auto reg : _regs)
    {
      auto &defs = reg->defs ();
      if (defs.size () == 1)
	if (CopyInsn *copy_insn = dynamic_cast<CopyInsn *> (defs.front ()))
	  {
	    // Where REG is copied from.
	    //
	    auto &copy_from = copy_insn->args ();
	    auto &copy_to = copy_insn->results ();

	    for (unsigned i = 0; i < copy_to.size (); i++)
	      if (copy_to[i] == reg)
		{
		  // REG_SRC is where REG is copied from.
		  //
		  // See if REG_SRC has a single definition, in which
		  // case the definition of REG_SRC must dominate COPY
		  // (otherwise the copy would be invalid), and thus
		  // transitively must dominate all uses of REG, as
		  // REG also has only a single definition, by COPY.
		  //
		  // When that is true, REG can simply be replaced by
		  // REG_SRC.

		  Reg *reg_src = copy_from[i];
		  if (reg_src->defs ().size () == 1)
		    {
		      // Replace all uses of REG by REG_SRC.  Note
		      // that doing this replacement in an instruction
		      // will remove a use of REG from REG_USES, and
		      // so we just iterate changing the first element
		      // of REG_USES until it is empty (iterating
		      // directly over REG_USES would be problematic
		      // because we're mutating the list at the same
		      // time).
		      //
		      auto &reg_uses = reg->uses ();
		      while (! reg_uses.empty ())
			reg_uses.front ()->change_arg (reg, reg_src);
		    }
		}
	  }
    }
}

// Remove all copy instructions whose results are unused.
//
void
Fun::remove_useless_copies ()
{
  for (auto bb : _blocks)
    {
      //
      // Use a funny loop (instead of "for (... : ...)") because we'd
      // like to remove elements while we're iterating over them.  So
      // we increment the list pointer before possibly removing what
      // it used to point to.
      //

      const std::list<Insn *> &insns = bb->insns ();

      auto insnp = insns.begin ();
      while (insnp != insns.end ())
	{
	  Insn *insn = *insnp;
	  ++insnp;

	  if (CopyInsn *copy_insn = dynamic_cast<CopyInsn *> (insn))
	    {
	      bool result_used = false;
	      for (auto result : copy_insn->results ())
		if (! result->uses ().empty ())
		  {
		    result_used = true;
		    break;
		  }

	      if (! result_used)
		delete copy_insn;
	    }
	}
    }
}

