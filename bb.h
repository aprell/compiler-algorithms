#ifndef __BB_H__
#define __BB_H__

#include <list>
#include <string>

#include "memtoobj.h"


class FlowGraph;


class BB
{
private:

  class DomTreeNode
  {
  public:

    // Assert that this node is immediately dominated by the dominator
    // tree node DOM.
    //
    void set_dominator (DomTreeNode *dom); 

    // Return the nearest common ancestor in the dominator tree which is
    // an ancestor of both this and OTHER.  If there is none, return 0.
    //
    DomTreeNode *common_ancestor (DomTreeNode *other);

    // Return true if this dominator tree node is an ancestor of
    // OTHER, i.e., we dominate OTHER.
    //
    bool is_ancestor_of (DomTreeNode *other)
    {
      while (other && other->depth > depth)
	other = other->dominator;

      return (other == this);
    }

    // Parent in dominator tree.
    //
    DomTreeNode *dominator;

    // Children in dominator tree.
    //
    std::list<DomTreeNode *> dominatees;

  private:

    // Set the depth of this node to DEPTH, and update all child nodes
    // recursively.
    //
    void update_depths (unsigned depth);

    // Depth in the dominator tree, with the root as 0.
    //
    unsigned depth;
  };


public:

  // Make a new block in flow graph FLOW_GRAPH.
  //
  BB (FlowGraph *flow_graph);

  // Return true if this block dominates BLOCK.
  //
  bool dominates (BB *block)
  {
    return fwd_dom_tree_node.is_ancestor_of (&block->fwd_dom_tree_node);
  }

  // Return true if this block is dominated by BLOCK.
  //
  bool is_dominated_by (BB *block)
  {
    return block->fwd_dom_tree_node.is_ancestor_of (&fwd_dom_tree_node);
  }

  // Return true if this block reverse-dominates BLOCK.
  //
  bool reverse_dominates (BB *block)
  {
    return rev_dom_tree_node.is_ancestor_of (&block->rev_dom_tree_node);
  }

  // Return true if this block is reverse-dominated by BLOCK.
  //
  bool is_reverse_dominated_by (BB *block)
  {
    return block->rev_dom_tree_node.is_ancestor_of (&rev_dom_tree_node);
  }

  // Return this block's immedately dominating block, or zero if none.
  //
  BB *dominator () const
  {
    return member_to_enclosing_object (fwd_dom_tree_node.dominator, &BB::fwd_dom_tree_node);
  }

  // Return this block's immedately dominating block, or zero if none.
  //
  BB *reverse_dominator () const
  {
    return member_to_enclosing_object (rev_dom_tree_node.dominator, &BB::rev_dom_tree_node);
  }


  // Calculate the forward dominator tree for all blocks in BLOCKS.
  //
  static void calc_dominators (const std::list<BB *> &blocks)
  {
    calc_doms (blocks, &BB::fwd_dom_tree_node, &BB::preds);
  }

  // Calculate the reverse dominator tree for all blocks in BLOCKS.
  //
  static void calc_reverse_dominators (const std::list<BB *> &blocks)
  {
    calc_doms (blocks, &BB::rev_dom_tree_node, &BB::succs);
  }


  // Return a text label for this block.  Only guaranteed to be unique
  // with in its flow-graph.
  //
  std::string label () { std::string label ("BB"); label += num; return label; }

  // Return a string containing labels for all blocks in BLOCK_LIST,
  // separated by a comma and space.
  //
  std::string block_list_labels (const std::list<BB *> &block_list);


  // Dump a text representation of this block to OUT.
  //
  void dump (std::ostream &out);


  // Unique block number, assigned at block creation time.
  //
  unsigned num;

  // Predecessor blocks of this block.
  //
  std::list<BB *> preds;

  // Successor blocks of this block.
  //
  std::list<BB *> succs;

  // Dominator-tree node for forward dominator tree.
  //
  DomTreeNode fwd_dom_tree_node;

  // Dominator-tree node for reverse dominator tree.
  //
  DomTreeNode rev_dom_tree_node;

  // Flow graph this block belongs to.
  //
  FlowGraph *flow_graph;


private:
  
  static void calc_doms (const std::list<BB *> &blocks,
			 DomTreeNode BB::*dom_tree_node_member,
			 std::list<BB *> BB::*pred_list_member);

};


#endif // __BB_H__