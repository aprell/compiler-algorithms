#include <fstream>

#include "bb.h"

#include "flow-graph-io.h"
#include "insn-io.h"

#include "bb-io.h"



BBDumper::BBDumper (FlowGraphDumper &_flow_graph_dumper)
  : flow_graph_dumper (_flow_graph_dumper) 
{
}


// Write a text representation of BLOCK.
//
void
BBDumper::dump (BB *block)
{
  std::ostream &out = flow_graph_dumper.out;
  InsnDumper &insn_dumper = flow_graph_dumper.insn_dumper;

  out << flow_graph_dumper.block_label (block) << ":\n";

  if (! block->predecessors ().empty ())
    out << "   # preds: "
	<< flow_graph_dumper.block_list_labels (block->predecessors ())
	<< '\n';

  BB *dom = block->dominator ();
  if (dom)
    out << "   # dominator: "
	<< flow_graph_dumper.block_label (dom)
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
	<< flow_graph_dumper.block_label (rev_dom)
	<< '\n';

  if (! block->successors ().empty ())
    out << "   # succs: "
	<< flow_graph_dumper.block_list_labels (block->successors ())
	<< '\n';
}

