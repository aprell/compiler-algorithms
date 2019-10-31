#ifndef __FLOW_GRAPH_H__
#define __FLOW_GRAPH_H__

#include <ios>

#include "bb.h"


class FlowGraph
{
public:

  // Calculate the forward dominator tree for all blocks in BLOCKS.
  //
  void calc_dominators () { BB::calc_dominators (_blocks); }

  // Calculate the reverse dominator tree for all blocks in BLOCKS.
  //
  void calc_reverse_dominators () { BB::calc_reverse_dominators (_blocks); }


  // Add new block to this flow graph.
  //
  void add_block (BB *block)
  {
    block->set_num (max_block_num++);
    _blocks.push_back (block);
  }

  // Return a reference to a read-only list containing the blocks in
  // this flow graph.
  //
  const std::list<BB *> &blocks () const { return _blocks; }


  // Return the entry block for this flow graph, which cannot have any
  // predecessors.
  //
  BB *entry_block () const { return _entry_block; }

  // Return the exit block for this flow graph, which cannot have any
  // successors.
  //
  BB *exit_block () const { return _exit_block; }


  unsigned max_block_num;


private:

  // Entry block for this flow graph.  Cannot have any predecessors.
  //
  BB *_entry_block;

  // Exit block for this flow graph.  Cannot have any successors.
  //
  BB *_exit_block;

  // List of basic blocks in this flow graph, in no particular order.
  //
  std::list<BB *> _blocks;
};


#endif // __FLOW_GRAPH_H__
