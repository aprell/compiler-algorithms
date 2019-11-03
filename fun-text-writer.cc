#include <ostream>
#include <unordered_set>
#include <deque>

#include "reg.h"
#include "fun.h"

#include "fun-text-writer.h"


FunTextWriter::FunTextWriter (std::ostream &_out)
  : out (_out), insn_writer (*this), block_writer (*this)
{
}


// Write a text representation of FUN.
//
void
FunTextWriter::write (Fun *fun)
{
  out << "{\n";

  // Mention which registers are defined here.
  //
  if (! fun->regs ().empty ())
    {
      out << "   regs ";

      bool first = true;
      for (auto reg : fun->regs ())
	{
	  if (first)
	    first = false;
	  else
	    out << ", ";
	  out << reg->name ();
	}

      out << '\n';
    }

  // A record of which blocks we've already written.
  //
  std::unordered_set<BB *> written_blocks;

  // A queue of blocks needing to be written.
  //
  std::deque<BB *> write_queue;

  // The exit block in the function.  We treat this specially so
  // that it's written last.
  //
  BB *exit_block = fun->exit_block ();

  // Start out by writing the entry point.
  //
  write_queue.push_back (fun->entry_block ());

  // Write the function in depth-first order, avoiding loops by just
  // ignoring any block we've already written.
  //
  while (! write_queue.empty ())
    {
      BB *block = write_queue.front ();
      write_queue.pop_front ();

      block_writer.write (block);

      written_blocks.insert (block);

      for (auto succ : block->successors ())
	if (succ != exit_block && written_blocks.find (succ) == written_blocks.end ())
	  write_queue.push_back (succ);
    }

  // Now if we haven't written the final exit block, do so.
  //
  if (exit_block && written_blocks.find (exit_block) == written_blocks.end ())
    block_writer.write (exit_block);

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
