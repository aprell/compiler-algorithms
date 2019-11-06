#ifndef __BB_H__
#define __BB_H__

#include <list>

#include "memtoobj.h"


class Fun;
class Insn;


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
    bool is_ancestor_of (const DomTreeNode *other) const
    {
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
  //
  bool dominates (BB *block) const
  {
    return fwd_dom_tree_node.is_ancestor_of (&block->fwd_dom_tree_node);
  }

  // Return true if this block is dominated by BLOCK.
  //
  bool is_dominated_by (BB *block) const
  {
    return block->fwd_dom_tree_node.is_ancestor_of (&fwd_dom_tree_node);
  }

  // Return true if this block post-dominates BLOCK.
  //
  bool post_dominates (BB *block) const
  {
    return bwd_dom_tree_node.is_ancestor_of (&block->bwd_dom_tree_node);
  }

  // Return true if this block is post-dominated by BLOCK.
  //
  bool is_post_dominated_by (BB *block) const
  {
    return block->bwd_dom_tree_node.is_ancestor_of (&bwd_dom_tree_node);
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


private:
  
  static void calc_doms (const std::list<BB *> &blocks,
			 DomTreeNode BB::*dom_tree_node_member,
			 std::list<BB *> BB::*pred_list_member);

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
