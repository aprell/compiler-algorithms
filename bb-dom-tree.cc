// bb-dom-tree.cc -- IR basic block dominator calculation
//
// Copyright © 2019  Miles Bader
//
// Author: Miles Bader <snogglethorpe@gmail.com>
// Created: 2019-10-28
//

#include "bb.h"


// Assert that this block is immediately dominated by DOM.
//
// DOM_TREE_NODE_MEMBER identifies which dominator tree node to use.
//
void
BB::set_dominator (BB *dom, DomTreeNode BB::*dom_tree_node_member)
{
  // If we already have a parent, remove ourselves from it.
  //
  // Note that we don't update the depth of our subtree at this point.
  //
  BB *old_dom = (this->*dom_tree_node_member).dominator;
  if (old_dom)
    {
      (old_dom->*dom_tree_node_member).dominatees.remove (this);
      (this->*dom_tree_node_member).dominator = 0;
    }

  // Add ourselves as a new child of DOM, and update our subtree
  // depths.
  //
  if (dom)
    {
      (dom->*dom_tree_node_member).dominatees.push_front (this);
      (this->*dom_tree_node_member).dominator = dom;

      update_dominator_depths ((dom->*dom_tree_node_member).depth + 1,
			       dom_tree_node_member);
    }
  else if ((dom->*dom_tree_node_member).depth != 0)
    {
      // We're now the root, and we weren't previously, so update
      // depths accordingly.

      update_dominator_depths (0, dom_tree_node_member);
    }
}


// Return the nearest common ancestor in the dominator tree which is
// an ancestor of both this block and OTHER.  If there is none, return 0.
//
// DOM_TREE_NODE_MEMBER identifies which dominator tree node to use.
//
BB *
BB::common_dominator (BB *other, DomTreeNode BB::*dom_tree_node_member)
{
  if (! other)
    return 0;

  BB *a = this, *b = other;

  while (a
	 && ((a->*dom_tree_node_member).depth
	     > (b->*dom_tree_node_member).depth))
    a = (a->*dom_tree_node_member).dominator;

  while (b
	 && ((b->*dom_tree_node_member).depth
	     > (a->*dom_tree_node_member).depth))
    b = (b->*dom_tree_node_member).dominator;

  while (a && a != b)
    {
      a = (a->*dom_tree_node_member).dominator;
      b = (b->*dom_tree_node_member).dominator;
    }

  return a;
}

// Set the dominator-tree depth of this block to DEPTH, and update
// all dominated blocks recursively.
//
// DOM_TREE_NODE_MEMBER identifies which dominator tree node to use.
//
void
BB::update_dominator_depths (unsigned depth,
			     DomTreeNode BB::*dom_tree_node_member)
{
  (this->*dom_tree_node_member).depth = depth;

  for (auto child : (this->*dom_tree_node_member).dominatees)
    child->update_dominator_depths (depth + 1, dom_tree_node_member);
}


// Remove this block from the dominator tree represented by
// DOM_TREE_NODE_MEMBER.
//
void
BB::remove_from_dominator_tree (DomTreeNode BB::*dom_tree_node_member)
{
  BB *dominator = (this->*dom_tree_node_member).dominator;

  for (auto dominee : (this->*dom_tree_node_member).dominatees)
    {
      (dominee->*dom_tree_node_member).dominator = dominator;

      if (dominator)
	(dominator->*dom_tree_node_member).dominatees.push_front (dominee);
    }
}


// Calculate the dominator tree for blocks in BLOCKS, using dominator
// node members DOM_TREE_NODE_MEMBER, and block-predecessor list
// members PRED_LIST_MEMBER.
//
void
BB::calc_doms (const std::list<BB *> &blocks,
	       DomTreeNode BB::*dom_tree_node_member,
	       std::list<BB *> BB::*pred_list_member)
{
  // Clear old dominator info.
  //
  for (auto block : blocks)
    {
      (block->*dom_tree_node_member).dominator = 0;
      (block->*dom_tree_node_member).dominatees.clear ();
    }

  //
  // Calculate new dominator info.
  //

  //
  // From Wikipedia:
  //
  //  // dominator of the start node is the start itself
  //  Dom(n0) = {n0}
  //
  //  // for all other nodes, set all nodes as the dominators
  //  for each n in N - {n0}
  //      Dom(n) = N;
  //
  //  // iteratively eliminate nodes that are not dominators
  //  while changes in any Dom(n)
  //      for each n in N - {n0}:
  //          Dom(n) = {n} union with intersection over Dom(p) for all p in pred(n)
  //

  bool change = true;
  while (change)
    {
      change = false;
      for (auto block : blocks)
	{
	  // Remember the previous dominator set for BLOCK.
	  //
	  BB *old_dom = (block->*dom_tree_node_member).dominator;

	  // The newly calculated dominator set.
	  //
	  BB *new_dom = 0;

	  // Go through all BLOCK's predecessors.
	  //
	  for (auto pred : block->*pred_list_member)
	    {
	      // If it wouldn't form a loop, try to update NEW_DOM
	      // based on PRED.
	      //
	      if (! block->dominates (pred, false, dom_tree_node_member))
		{
		  if (! new_dom)
		    // PRED is the first predecessor, so the dominator
		    // set represented by NEW_DOM is the same as
		    // PRED's dominator set.
		    //
		    new_dom = pred;
		  else
		    // Otherwise, take the intersection of PRED's
		    // dominator set with previously NEW_DOM (which
		    // currently represents the dominator set found
		    // for all predecessors of BLOCK so far).
		    //
		    new_dom = new_dom->common_dominator (pred,
							 dom_tree_node_member);
		}
	    }

	  // If the dominator set has changed, remember that.
	  //
	  if (new_dom != old_dom)
	    {
	      block->set_dominator (new_dom, dom_tree_node_member);
	      change = true;
	    }
	}
    }
}


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
void
BB::extend_dominance_frontier (const BB *dom_tree_root,
			       DomTreeNode BB::*dom_tree_node_member,
			       std::list<BB *> BB::*succ_list_member,
			       std::list<BB *> &frontier)
  const
{
  // Basically just walk the dominator tree, seeing which successors
  // of each block are not descendents of DOM_TREE_ROOT.

  // Check the successors of this node to see if any are not
  // descendents of DOM_TREE_ROOT in the dominator tree; in other
  // words, they're in the dominance frontier of DOM_TREE_ROOT, having
  // "escaped" the region dominated by DOM_TREE_ROOT.
  //
  for (auto succ : this->*succ_list_member)
    // If DOM_TREE_ROOT does not strictly dominated succ_node, then
    // SUCC_NODE is in the dominance frontier of DOM_TREE_ROOT.
    //
    if (! dom_tree_root->dominates (succ, true, dom_tree_node_member))
      {
	// Only add SUCC to FRONTIER if it's not a duplicate.
	//
	if (std::find (frontier.begin(), frontier.end(), succ) == frontier.end ())
	  frontier.push_back (succ);
      }

  // Now walk the rest of the dominator tree underneath this node.
  //
  for (auto dominatee : (this->*dom_tree_node_member).dominatees)
    dominatee->extend_dominance_frontier (dom_tree_root,
					  dom_tree_node_member, succ_list_member, 
					  frontier);
}
