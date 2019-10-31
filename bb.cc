#include <ostream>

#include "bb.h"
#include "insn.h"

#include "flow-graph.h"


// Make a new block in flow graph FLOW_GRAPH.
//
BB::BB (FlowGraph *flow_graph)
  : _flow_graph (flow_graph)
{
  flow_graph->add_block (this);
}


// Add the instruction INSN to the end of this block.
//
void
BB::add_insn (Insn *insn)
{
  BB *old_block = insn->block ();

  if (old_block)
    old_block->remove_insn (insn);

  _insns.push_back (insn);

  insn->set_block (this);
}

// Remove the instruction INSN from this block.
//
void
BB::remove_insn (Insn *insn)
{
  _insns.remove (insn);
  insn->set_block (0);
}


// Set the block to which control-flow in this block goes if
// execution runs off the end of it to BLOCK.  BLOCK may be zero.
//
void
BB::set_fall_through (BB *block)
{
  if (block != _fall_through)
    {
      if (_fall_through)
	remove_successor (_fall_through);

      if (block)
	add_successor (block);

      _fall_through = block;
    }
}


// Add a new control flow edge reflecting that SUCC is now a
// successor of this block.
///
void
BB::add_successor (BB *succ)
{
  _succs.push_back (succ);
  succ->_preds.push_back (this);
}

// Remove a control flow edge between this block and the previous
// successor block SUCC.
///
void
BB::remove_successor (BB *succ)
{
  _succs.remove (succ);
  succ->_preds.remove (this);
}
