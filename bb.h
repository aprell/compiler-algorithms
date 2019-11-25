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


class Fun;
class Insn;


// IR basic block in a flow graph.
//
class BB
{
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
    return dominates (block, strictly, &BB::fwd_dom_tree_node);
  }

  // Return true if this block is dominated by BLOCK.
  // If STRICTLY is false, then a block dominates itself, otherwise it
  // does not.
  //
  bool is_dominated_by (BB *block, bool strictly = false) const
  {
    return block->dominates (this, strictly, &BB::fwd_dom_tree_node);
  }

  // Return true if this block post-dominates BLOCK.
  // If STRICTLY is false, then a block dominates itself, otherwise it
  // does not.
  //
  bool post_dominates (BB *block, bool strictly = false) const
  {
    return dominates (block, strictly, &BB::bwd_dom_tree_node);
  }

  // Return true if this block is post-dominated by BLOCK.
  // If STRICTLY is false, then a block dominates itself, otherwise it
  // does not.
  //
  bool is_post_dominated_by (BB *block, bool strictly = false) const
  {
    return block->dominates (this, strictly, &BB::bwd_dom_tree_node);
  }


  // Return this block's immedately dominating block, or zero if none.
  //
  BB *dominator () const { return fwd_dom_tree_node.dominator; }

  // Return this block's immedately dominating block, or zero if none.
  //
  BB *post_dominator () const { return bwd_dom_tree_node.dominator; }


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
    extend_dominance_frontier (this,
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


  //
  // Dominator info
  //


  // Dominator tree info.
  //
  struct DomTreeNode
  {
    // Parent in dominator tree.
    //
    BB *dominator = 0;

    // Children in dominator tree.
    //
    std::list<BB *> dominatees;

    // Depth in the dominator tree, with the root as 0.
    //
    unsigned depth = 0;
  };


  // Mark dominator / post-dominator information in this
  // block's function as out of date.
  //
  void invalidate_dominators ();
  void invalidate_post_dominators ();


  // Return true if this block dominates OTHER.  If STRICTLY is false,
  // then a node is considered to be an ancestor of itself; otherwise,
  // it is not.
  //
  // DOM_TREE_NODE_MEMBER identifies which dominator tree node to use.
  //
  bool dominates (const BB *other, bool strictly,
		  DomTreeNode BB::*dom_tree_node_member)
    const
  {
    if (strictly && this == other)
      return false;

    while (other
	   && ((other->*dom_tree_node_member).depth
	       > (this->*dom_tree_node_member).depth))
      other = (other->*dom_tree_node_member).dominator;

    return (other == this);
  }

  // Assert that this block is immediately dominated by DOM.
  //
  // DOM_TREE_NODE_MEMBER identifies which dominator tree node to use.
  //
  void set_dominator (BB *dom, DomTreeNode BB::*dom_tree_node_member);

  // Return the nearest common ancestor in the dominator tree which is
  // an ancestor of both this block and OTHER.  If there is none, return 0.
  //
  // DOM_TREE_NODE_MEMBER identifies which dominator tree node to use.
  //
  BB *common_dominator (BB *other, DomTreeNode BB::*dom_tree_node_member);

  // Set the dominator-tree depth of this block to DEPTH, and update
  // all dominated blocks recursively.
  //
  // DOM_TREE_NODE_MEMBER identifies which dominator tree node to use.
  //
  void update_dominator_depths (unsigned depth,
				DomTreeNode BB::*dom_tree_node_member);

  // Remove this block from the dominator tree represented by
  // DOM_TREE_NODE_MEMBER.
  //
  void remove_from_dominator_tree (DomTreeNode BB::*dom_tree_node_member);


  // Calculate the dominator tree for blocks in BLOCKS, using dominator
  // node members DOM_TREE_NODE_MEMBER, and block-predecessor list
  // members PRED_LIST_MEMBER.
  //
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
  void extend_dominance_frontier (const BB *dom_tree_root,
				  DomTreeNode BB::*dom_tree_node_member,
				  std::list<BB *> BB::*succ_list_member,
				  std::list<BB *> &frontier)
    const;


  // Dominator-tree node for forward dominator tree.
  //
  DomTreeNode fwd_dom_tree_node;

  // Dominator-tree node for post dominator tree.
  //
  DomTreeNode bwd_dom_tree_node;

};


#endif // __BB_H__
