// bb-text-writer.cc -- Text-format output of an IR basic block
//
// Copyright © 2019  Miles Bader
//
// Author: Miles Bader <snogglethorpe@gmail.com>
// Created: 2019-11-02
//

#include <ostream>

#include "fun.h"
#include "bb.h"

#include "fun-text-writer.h"
#include "insn-text-writer.h"

#include "bb-text-writer.h"



BBTextWriter::BBTextWriter (FunTextWriter &_fun_writer)
  : fun_writer (_fun_writer) 
{
}


// Write a text representation of BLOCK.  If NEXT_BLOCK is non-zero,
// it is the next block that will be written; this is used to suppress
// a goto statement between adjacent blocks.
//
void
BBTextWriter::write (BB *block, BB *next_block)
{
  std::ostream &out = fun_writer.output_stream ();
  InsnTextWriter &insn_writer = fun_writer.insn_writer;

  out << fun_writer.block_label (block) << "\n";

  Fun *fun = block->fun ();

  if (block == fun->entry_block ())
    out << "   # entry\n";
  if (block == fun->exit_block ())
    out << "   # exit\n";

  if (! block->predecessors ().empty ())
    out << "   # preds: "
	<< fun_writer.block_list_labels (block->predecessors ())
	<< '\n';

  BB *dom = block->dominator ();
  if (dom)
    {
      out << "   # dominator: "
	  << fun_writer.block_label (dom)
	  << '\n';
      out << "   # dominance frontier: "
	  << fun_writer.block_list_labels (block->dominance_frontier ())
	  << '\n';
    }

  for (auto insn : block->insns ())
    {
      out << "   ";
      insn_writer.write (insn);
      out << '\n';
    }

  if (block->fall_through () && block->fall_through () != next_block)
    out << "   goto " << fun_writer.block_label (block->fall_through ()) << '\n';

  BB *rev_dom = block->post_dominator ();
  if (rev_dom)
    out << "   # post-dominator: "
	<< fun_writer.block_label (rev_dom)
	<< '\n';

  if (! block->successors ().empty ())
    out << "   # succs: "
	<< fun_writer.block_list_labels (block->successors ())
	<< '\n';
}

