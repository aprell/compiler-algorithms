#include <deque>

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
      for (auto bb : blocks)
	{
	  // Remember the previous dominator.
	  //
	  DomTreeNode *old_dom = (bb->*dom_tree_node_member).dominator;

	  // Calculate a new one.
	  //
	  DomTreeNode *new_dom = 0;
	  for (auto prev : bb->*pred_list_member)
	    {
	      DomTreeNode *prev_dom = &(prev->*dom_tree_node_member);
	      if (! new_dom)
		new_dom = prev_dom;
	      else
		new_dom = new_dom->common_ancestor (prev_dom);
	    }

	  if (new_dom != old_dom)
	    {
	      (bb->*dom_tree_node_member).set_dominator (new_dom);
	      change = true;
	    }
	}
    }
}
