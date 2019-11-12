// bb-dom-tree.cc -- IR basic block dominator calculation
//
// Copyright © 2019  Miles Bader
//
// Author: Miles Bader <snogglethorpe@gmail.com>
// Created: 2019-10-28
//

#include "bb.h"


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



// Assert that this node is immediately dominated by the dominator
// tree node DOM.
//
void
BB::DomTreeNode::set_dominator (DomTreeNode *dom)
{
  // If we already have a parent, remove ourselves from it.
  //
  // Note that we don't update the depth of our subtree at this point.
  //
  if (dominator)
    {
      dominator->dominatees.remove (this);
      dominator = 0;
    }

  // Add ourselves as a new child of DOM, and update our subtree
  // depths.
  //
  if (dom)
    {
      dom->dominatees.push_front (this);
      dominator = dom;

      update_depths (dom->depth + 1);
    }
  else if (depth != 0)
    {
      // We're now the root, and we weren't previously, so update
      // depths accordingly.

      update_depths (0);
    }
}


// Set the depth of this node to _DEPTH, and update all child nodes
// recursively.
//
void
BB::DomTreeNode::update_depths (unsigned _depth)
{
  depth = _depth;

  for (auto child : dominatees)
    child->update_depths (_depth + 1);
}


// Return the nearest common ancestor in the dominator tree which is
// an ancestor of both this and OTHER.  If there is none, return 0.
//
BB::DomTreeNode *
BB::DomTreeNode::common_ancestor (DomTreeNode *other)
{
  if (! other)
    return 0;

  DomTreeNode *a = this, *b = other;

  while (a && a->depth > b->depth)
    a = a->dominator;

  while (b && b->depth > a->depth)
    b = b->dominator;

  while (a && a != b)
    {
      a = a->dominator;
      b = b->dominator;
    }

  return a;
}



void
BB::calc_doms (const std::list<BB *> &blocks,
	       DomTreeNode BB::*dom_tree_node_member,
	       std::list<BB *> BB::*pred_list_member)
{
  bool change = true;
  while (change)
    {
      change = false;
      for (auto block : blocks)
	{
	  DomTreeNode *block_node = &(block->*dom_tree_node_member);

	  // Remember the previous dominator set for BLOCK.
	  //
	  DomTreeNode *old_dom = block_node->dominator;

	  // The newly calculated dominator set.
	  //
	  DomTreeNode *new_dom = 0;

	  // Go through all BLOCK's predecessors.
	  //
	  for (auto pred : block->*pred_list_member)
	    {
	      DomTreeNode *pred_node = &(pred->*dom_tree_node_member);

	      // If it wouldn't form a loop, try to update NEW_DOM
	      // based on PRED.
	      //
	      if (! block_node->is_ancestor_of (pred_node))
		{
		  if (! new_dom)
		    // PRED is the first predecessor, so the dominator
		    // set represented by NEW_DOM is the same as
		    // PRED's dominator set.
		    //
		    new_dom = pred_node;
		  else
		    // Otherwise, take the intersection of PRED's
		    // dominator set with previously NEW_DOM (which
		    // currently represents the dominator set found
		    // for all predecessors of BLOCK so far).
		    //
		    new_dom = new_dom->common_ancestor (pred_node);
		}
	    }

	  // If the dominator set has changed, remember that.
	  //
	  if (new_dom != old_dom)
	    {
	      block_node->set_dominator (new_dom);
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
BB::extend_dominance_frontier (const DomTreeNode *dom_tree_root,
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
    {
      // Dominator tree node in SUCC.
      //
      DomTreeNode *succ_node = &(succ->*dom_tree_node_member);

      // If DOM_TREE_ROOT does not strictly dominated succ_node, then
      // SUCC_NODE is in the dominance frontier of DOM_TREE_ROOT.
      //
      if (succ_node == dom_tree_root
	  || ! dom_tree_root->is_ancestor_of (succ_node))
	{
	  // Only add SUCC to FRONTIER if it's not a duplicate.
	  //
	  if (std::find (frontier.begin(), frontier.end(), succ) == frontier.end ())
	    frontier.push_back (succ);
	}
    }

  // Now walk the rest of the dominator tree underneath this node.
  //
  for (auto dominatee_node : (this->*dom_tree_node_member).dominatees)
    member_to_enclosing_object (dominatee_node, dom_tree_node_member)
      ->extend_dominance_frontier (dom_tree_root,
				   dom_tree_node_member, succ_list_member, 
				   frontier);
}
