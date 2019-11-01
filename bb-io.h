#ifndef __BB_IO_H__
#define __BB_IO_H__


class BB;
class FlowGraphDumper;


// A class for outputting text representations of a basic block.
//
class BBDumper
{
public:

  BBDumper (FlowGraphDumper &flow_graph_dumper);

  // Write a text representation of BLOCK.
  //
  void dump (BB *block);


private:

  // Dumper for the flow graph we're associated with.
  //
  FlowGraphDumper &flow_graph_dumper;
};


#endif // __BB_IO_H__
