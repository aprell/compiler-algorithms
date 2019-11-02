#ifndef __FUN_IO_H__
#define __FUN_IO_H__

#include <string>

#include "insn-io.h"
#include "bb-io.h"


class Fun;


// A class for outputting text representations of a function.
//
class FunDumper
{
public:

  FunDumper (std::ostream &out);

  // Write a text representation of FUN.
  //
  void dump (Fun *fun);


  // Return a text label for BLOCK.
  //
  std::string block_label (const BB *block);

  // Return a string containing labels for all blocks in BLOCK_LIST,
  // separated by a comma and space.
  //
  std::string block_list_labels (const std::list<BB *> &block_list);


  //
  // The following data members are all public, and reflect common
  // state during dumping.
  //


  // Streamer we're dumping to.
  //
  std::ostream &out;

  // Dumper for insns.
  //
  InsnDumper insn_dumper;

  // Dumper for blocks.
  //
  BBDumper block_dumper;
};


#endif // __FUN_IO_H__
