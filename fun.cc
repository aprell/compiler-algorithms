#include <deque>

#include "fun.h"


// For any blocks which have no predecessors, remove their outgoing
// edges.
//
void
Fun::propagate_unreachable ()
{
  std::deque<BB *> queue;
  for (auto bb : _blocks)
    if (bb != _entry_block && bb->predecessors ().empty ())
      queue.push_back (bb);

  while (! queue.empty ())
    {
      BB *bb = queue.front ();
      queue.pop_front ();

      //
      // Use a funny loop (instead of "for (... : ...)") because we'd
      // like to remove elements while we're iterator over them.  So
      // we increment the list pointer before possibly removing what
      // it used to point to.
      //

      const std::list<BB *> &succs = bb->successors ();

      auto succp = succs.begin ();
      while (succp != succs.end ())
	{
	  BB *succ = *succp;
	  ++succp;

	  bb->remove_successor (succ);

	  if (succ->predecessors ().empty ())
	    queue.push_back (succ);
	}
    }
}
