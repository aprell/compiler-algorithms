// fun-text-writer.cc -- Text-format output of an IR function
//
// Copyright © 2019  Miles Bader
//
// Author: Miles Bader <snogglethorpe@gmail.com>
// Created: 2019-11-02
//

#include <ostream>
#include <unordered_set>
#include <deque>

#include "reg.h"
#include "fun.h"

#include "prog-text-writer.h"
#include "fun-text-writer.h"


FunTextWriter::FunTextWriter (ProgTextWriter &_prog_writer)
  : prog_writer (_prog_writer), insn_writer (*this), block_writer (*this)
{
}


// Return the output stream we're writing to.
//
std::ostream
&FunTextWriter::output_stream ()
{
  return prog_writer.out ();
}



// Write a text representation of FUN.
//
void
FunTextWriter::write (Fun *fun)
{
  std::ostream &out = output_stream ();

  out << "{\n";

  // Mention which registers are defined here.
  //
  for (auto reg : fun->regs ())
    {
      const std::string &name = reg->name ();
      unsigned name_len = name.size ();
      std::string padding ((name_len > 18 ? 2 : 20 - name_len), ' ');
      unsigned nuses = reg->uses ().size ();
      unsigned ndefs = reg->defs ().size ();
      out << "   reg " << name
	  << padding
	  << "# (" << nuses << (nuses == 1 ? " use" : " uses")
	  << ", " << ndefs << (ndefs == 1 ? " def" : " defs")
	  << ")\n";
    }

  // A record of which blocks we've queued to be written.
  //
  std::unordered_set<BB *> queued_blocks;

  // A queue of blocks needing to be written.
  //
  std::deque<BB *> write_queue;

  // The exit block in the function.  We treat this specially so
  // that it's written last.
  //
  BB *exit_block = fun->exit_block ();

  // Start out by writing the entry point.
  //
  if (fun->entry_block ())
    {
      write_queue.push_back (fun->entry_block ());
      queued_blocks.insert (fun->entry_block ());
    }

  // Write the function in depth-first order, avoiding loops by just
  // ignoring any block we've already written.
  //
  while (! write_queue.empty ())
    {
      BB *block = write_queue.front ();
      write_queue.pop_front ();

      BB *fall_through = block->fall_through ();

      // Prefer fall-through block next.
      //
      if (fall_through
	  && fall_through != exit_block
	  && queued_blocks.count (fall_through) == 0)
	{
	  write_queue.push_front (fall_through);
	  queued_blocks.insert (fall_through);
	}

      // Try to emit successor blocks
      for (auto succ : block->successors ())
	if (succ != exit_block
	    && succ != fall_through
	    && queued_blocks.count (succ) == 0)
	  {
	    write_queue.push_back (succ);
	    queued_blocks.insert (succ);
	  }

      // Insert the exit block into the queue if we're finally done.
      if (block != exit_block && exit_block && write_queue.empty ())
	write_queue.push_back (exit_block);

      BB *next_block = write_queue.empty () ? 0 : write_queue.front ();

      block_writer.write (block, next_block);
    }

  out << "}\n";
}


// Return a text label for BLOCK.
//
std::string
FunTextWriter::block_label (const BB *block)
{
  std::string label ("_");
  label += std::to_string (block->num ());
  return label;
}


// Return a string containing labels for all blocks in BLOCK_LIST,
// separated by a comma and space.
//
std::string
FunTextWriter::block_list_labels (const std::list<BB *> &block_list)
{
  std::string str;

  bool first = true;
  for (auto bb : block_list)
    {
      if (first)
	first = false;
      else
	str += ", ";

      str += block_label (bb);
    }

  return str;
}
