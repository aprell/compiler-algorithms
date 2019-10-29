#include <ostream>

#include "bb.h"

#include "flow-graph.h"


// Make a new block in flow graph FLOW_GRAPH.
//
BB::BB (FlowGraph *_flow_graph)
  : flow_graph (_flow_graph)
{
  _flow_graph->add_block (this);
}



// Return a string containing labels for all blocks in BLOCK_LIST,
// separated by a comma and space.
//
std::string
BB::block_list_labels (const std::list<BB *> &block_list)
{
  std::string str;

  bool first = true;
  for (auto bb : block_list)
    {
      if (first)
	first = false;
      else
	str += ", ";

      str += bb->label ();
    }

  return str;
}


// Dump a text representation of the flow graph to OUT.
//
void
BB::dump (std::ostream &out)
{
  out << label () << ":\n";

  if (! preds.empty ())
    out << "   # preds: " << block_list_labels (preds) << '\n';

  BB *dom = dominator ();
  if (dom)
    out << "   # dominator: " << dom->label () << '\n';

  BB *rev_dom = reverse_dominator ();
  if (rev_dom)
    out << "   # reverse-dominator: " << rev_dom->label () << '\n';

  if (! succs.empty ())
    out << "   # succs: " << block_list_labels (succs) << '\n';
}
