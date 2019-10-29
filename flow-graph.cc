#include <ostream>

#include "flow-graph.h"


// Dump a text representation of the flow graph to OUT.
//
void
FlowGraph::dump (std::ostream &out)
{
  for (auto bb : blocks)
    {
      bb->dump (out);
      out << '\n';
    }
}
