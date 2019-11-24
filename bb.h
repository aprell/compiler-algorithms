// bb.h -- IR basic blocks
//
// Copyright © 2019  Miles Bader
//
// Author: Miles Bader <snogglethorpe@gmail.com>
// Created: 2019-10-28
//

#ifndef __BB_H__
#define __BB_H__

#include <list>

#include "memtoobj.h"


class Fun;
class Insn;


// IR basic block in a flow graph.
//
class BB
{
private:

  class DomTreeNode
  {
  public:

    // Dtor tries to maintain the tree.
    //
    ~DomTreeNode ();

    // Assert that this node is immediately dominated by the dominator
    // tree node DOM.
    //
    void set_dominator (DomTreeNode *dom); 

    // Return the nearest common ancestor in the dominator tree which is
    // an ancestor of both this and OTHER.  If there is none, return 0.
    //
    DomTreeNode *common_ancestor (DomTreeNode *other);

    // Return true if this dominator tree node is an ancestor of
    // OTHER, i.e., we dominate OTHER.  If STRICTLY is false, then a
    // node is considered to be an ancestor of itself; otherwise, it
    // is not.
    //
    bool is_ancestor_of (const DomTreeNode *other, bool strictly = false)
      const
    {
      if (strictly && this == other)
	return false;

      while (other && other->depth > depth)
	other = other->dominator;

      return (other == this);
    }

    // Parent in dominator tree.
    //
    DomTreeNode *dominator = 0;

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
    unsigned depth = 0;
  };


public:

  // Make a new block in function FUN.
  //
  BB (Fun *fun);

  // Destructor; tries to cleanly remove the block from the flow
  // graph.
  //
  ~BB ();


  // Return the function this block belongs to.
  //
  Fun *fun () const { return _fun; }

  // Return this block's unique (within its fun) number.
  //
  unsigned num () const { return _num; }

  // Set this block's unique (within its function) number to NUM.
  //
  void set_num (unsigned num) { _num = num; }


  // Return true if this block is empty (contains no insns).
  //
  bool is_empty () const { return _insns.empty (); }


  //
  // Dominator-tree methods.
  //

  // Return true if this block dominates BLOCK.
  // If STRICTLY is false, then a block dominates itself, otherwise it
  // does not.
  //
  bool dominates (BB *block, bool strictly = false) const
  {
    return fwd_dom_tree_node.is_ancestor_of (&block->fwd_dom_tree_node,
					     strictly);
  }

  // Return true if this block is dominated by BLOCK.
  // If STRICTLY is false, then a block dominates itself, otherwise it
  // does not.
  //
  bool is_dominated_by (BB *block, bool strictly = false) const
  {
    return block->fwd_dom_tree_node.is_ancestor_of (&fwd_dom_tree_node,
						    strictly);
  }

  // Return true if this block post-dominates BLOCK.
  // If STRICTLY is false, then a block dominates itself, otherwise it
  // does not.
  //
  bool post_dominates (BB *block, bool strictly = false) const
  {
    return bwd_dom_tree_node.is_ancestor_of (&block->bwd_dom_tree_node,
					     strictly);
  }

  // Return true if this block is post-dominated by BLOCK.
  // If STRICTLY is false, then a block dominates itself, otherwise it
  // does not.
  //
  bool is_post_dominated_by (BB *block, bool strictly = false) const
  {
    return block->bwd_dom_tree_node.is_ancestor_of (&bwd_dom_tree_node,
						    strictly);
  }

  // Return this block's immedately dominating block, or zero if none.
  //
  BB *dominator () const
  {
    DomTreeNode *dom_node = fwd_dom_tree_node.dominator;
    if (dom_node)
      return member_to_enclosing_object (dom_node, &BB::fwd_dom_tree_node);
    else
      return 0;
  }

  // Return this block's immedately dominating block, or zero if none.
  //
  BB *post_dominator () const
  {
    DomTreeNode *bwd_dom_node = bwd_dom_tree_node.dominator;
    if (bwd_dom_node)
      return member_to_enclosing_object (bwd_dom_node, &BB::bwd_dom_tree_node);
    else
      return 0;
  }

  // Calculate the forward dominator tree for all blocks in BLOCKS.
  //
  static void calc_dominators (const std::list<BB *> &blocks)
  {
    calc_doms (blocks, &BB::fwd_dom_tree_node, &BB::_preds);
  }

  // Calculate the post dominator tree for all blocks in BLOCKS.
  //
  static void calc_post_dominators (const std::list<BB *> &blocks)
  {
    calc_doms (blocks, &BB::bwd_dom_tree_node, &BB::_succs);
  }


  // Return the dominance frontier of this block: all blocks which are
  // immediate successors of some block dominated by this block, but
  // are not dominated by this block themselves.
  //
  std::list<BB *> dominance_frontier () const
  {
    std::list<BB *> frontier;
    extend_dominance_frontier (&fwd_dom_tree_node,
			       &BB::fwd_dom_tree_node, &BB::_succs,
			       frontier);
    return frontier;
  }


  // Return the block to which control-flow in this block goes if
  // execution runs off the end of it.
  //
  BB *fall_through () const { return _fall_through; }

  // Set the block to which control-flow in this block goes if
  // execution runs off the end of it to BLOCK.  BLOCK may be zero.
  //
  void set_fall_through (BB *block);


  // Add a new control flow edge reflecting that SUCC is now a
  // successor of this block.
  ///
  void add_successor (BB *succ);

  // Remove a control flow edge between this block and the previous
  // successor block SUCC.
  ///
  void remove_successor (BB *succ);


  // Add the instruction INSN to the end of this block.
  //
  void add_insn (Insn *insn);

  // Add the instruction INSN to the end of this block, but before any
  // branch instructio if there is one.
  //
  void add_insn_before_branch (Insn *insn);

  // Add the instruction INSN to the beginning of this block.
  //
  void prepend_insn (Insn *insn);

  // Remove the instruction INSN from this block.
  //
  void remove_insn (Insn *insn);


  // Return a reference to a read-only list containing the predecessor
  // blocks of this block.
  //
  const std::list<BB *> &predecessors () const { return _preds; }

  // Return a reference to a read-only list containing the successor
  // blocks of this block.
  //
  const std::list<BB *> &successors () const { return _succs; }


  // Return a reference to a read-only list containing the
  // instructions in this block.
  //
  const std::list<Insn *> &insns () const { return _insns; }


  // Replace FROM in the successors of this block with TO.  TO may be
  // NULL, in which case the target becomes undefined.
  //
  void change_successor (BB *from, BB *to);


private:

  // Mark dominator / post-dominator information in this
  // block's function as out of date.
  //
  void invalidate_dominators ();
  void invalidate_post_dominators ();

  
  static void calc_doms (const std::list<BB *> &blocks,
			 DomTreeNode BB::*dom_tree_node_member,
			 std::list<BB *> BB::*pred_list_member);

  // Helper method used by BB::dominance_frontier method.
  //
  // Walk this node's dominator tree using dominator node members
  // DOM_TREE_NODE_MEMBER, and for every dominated node, add to
  // FRONTIER any successor of that node, using SUCC_LIST_MEMBER to
  // get the successors list, which is not a descendent of
  // DOM_TREE_ROOT.
  //
  // Nodes are only added to FRONTIER once, duplicates are ignored.
  //
  void extend_dominance_frontier (const DomTreeNode *dom_tree_root,
				  DomTreeNode BB::*dom_tree_node_member,
				  std::list<BB *> BB::*succ_list_member,
				  std::list<BB *> &frontier)
    const;


  // Unique block number, assigned at block creation time.
  //
  unsigned _num = 0;

  // Instructions in this block;
  //
  std::list<Insn *> _insns;

  // Where control-flow in this block goes if execution runs off the
  // end of it.
  //
  BB *_fall_through = 0;

  // function this block belongs to.
  //
  Fun *_fun = 0;


  // Predecessor blocks of this block.
  //
  std::list<BB *> _preds;

  // Successor blocks of this block.
  //
  std::list<BB *> _succs;


  // Dominator-tree node for forward dominator tree.
  //
  DomTreeNode fwd_dom_tree_node;

  // Dominator-tree node for post dominator tree.
  //
  DomTreeNode bwd_dom_tree_node;

};


#endif // __BB_H__
