#ifndef __FLOW_GRAPH_H__
#define __FLOW_GRAPH_H__

#include <ios>

#include "bb.h"


class FlowGraph
{
public:

  // Calculate the forward dominator tree for all blocks in BLOCKS.
  //
  void calc_dominators () { BB::calc_dominators (blocks); }

  // Calculate the reverse dominator tree for all blocks in BLOCKS.
  //
  void calc_reverse_dominators () { BB::calc_reverse_dominators (blocks); }


  // Dump a text representation of the flow graph to OUT.
  //
  void dump (std::ostream &out);


  // Add new block to this flow graph.
  //
  void add_block (BB *block)
  {
    block->num = max_block_num++;
    blocks.push_back (block);
  }


  unsigned max_block_num;

  BB *entry;

  BB *exit;

  std::list<BB *> blocks;
};


#endif // __FLOW_GRAPH_H__
