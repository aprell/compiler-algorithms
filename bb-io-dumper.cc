#include <fstream>

#include "bb.h"

#include "fun-io.h"
#include "insn-io.h"

#include "bb-io.h"



BBDumper::BBDumper (FunDumper &_fun_dumper)
  : fun_dumper (_fun_dumper) 
{
}


// Write a text representation of BLOCK.
//
void
BBDumper::dump (BB *block)
{
  std::ostream &out = fun_dumper.out;
  InsnDumper &insn_dumper = fun_dumper.insn_dumper;

  out << fun_dumper.block_label (block) << ":\n";

  if (! block->predecessors ().empty ())
    out << "   # preds: "
	<< fun_dumper.block_list_labels (block->predecessors ())
	<< '\n';

  BB *dom = block->dominator ();
  if (dom)
    out << "   # dominator: "
	<< fun_dumper.block_label (dom)
	<< '\n';

  for (auto insn : block->insns ())
    {
      out << "   ";
      insn_dumper.dump (insn);
      out << '\n';
    }

  BB *rev_dom = block->reverse_dominator ();
  if (rev_dom)
    out << "   # reverse-dominator: "
	<< fun_dumper.block_label (rev_dom)
	<< '\n';

  if (! block->successors ().empty ())
    out << "   # succs: "
	<< fun_dumper.block_list_labels (block->successors ())
	<< '\n';
}

