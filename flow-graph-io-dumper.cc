#include <ostream>
#include <unordered_set>
#include <deque>

#include "reg.h"
#include "flow-graph.h"

#include "flow-graph-io.h"


FlowGraphDumper::FlowGraphDumper (std::ostream &_out)
  : out (_out), insn_dumper (*this), block_dumper (*this)
{
}


// Write a text representation of FLOW_GRAPH.
//
void
FlowGraphDumper::dump (FlowGraph *flow_graph)
{
  // Mention which registers are defined here.
  //
  if (! flow_graph->regs ().empty ())
    {
      out << "   regs ";

      bool first = true;
      for (auto reg : flow_graph->regs ())
	{
	  if (first)
	    first = false;
	  else
	    out << ", ";
	  out << reg->name ();
	}

      out << '\n';
    }

  // A record of which blocks we've already dumped.
  //
  std::unordered_set<BB *> dumped_blocks;

  // A queue of blocks needing to be dumped.
  //
  std::deque<BB *> dump_queue;

  // The exit block in the flow graph.  We treat this specially so
  // that it's dumped last.
  //
  BB *exit_block = flow_graph->exit_block ();

  // Start out by dumping the entry point.
  //
  dump_queue.push_back (flow_graph->entry_block ());

  // Dump the flow graph in depth-first order, avoiding loops by just
  // ignoring any block we've already dumped.
  //
  while (! dump_queue.empty ())
    {
      BB *block = dump_queue.front ();
      dump_queue.pop_front ();

      block_dumper.dump (block);

      dumped_blocks.insert (block);

      for (auto succ : block->successors ())
	if (succ != exit_block && dumped_blocks.find (succ) == dumped_blocks.end ())
	  dump_queue.push_back (succ);
    }

  // Now if we haven't dumped the final exit block, do so.
  //
  if (exit_block && dumped_blocks.find (exit_block) == dumped_blocks.end ())
    block_dumper.dump (exit_block);
}


// Return a text label for BLOCK.
//
std::string
FlowGraphDumper::block_label (const BB *block)
{
  std::string label ("_");
  label += std::to_string (block->num ());
  return label;
}


// Return a string containing labels for all blocks in BLOCK_LIST,
// separated by a comma and space.
//
std::string
FlowGraphDumper::block_list_labels (const std::list<BB *> &block_list)
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
