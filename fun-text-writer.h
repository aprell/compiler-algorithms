// fun-text-writer.h -- Text-format output of an IR function
//
// Copyright © 2019  Miles Bader
//
// Author: Miles Bader <snogglethorpe@gmail.com>
// Created: 2019-11-02
//

#ifndef __FUN_TEXT_WRITER_H__
#define __FUN_TEXT_WRITER_H__

#include <string>

#include "insn-text-writer.h"
#include "bb-text-writer.h"


class Fun;


// A class for outputting text representations of a function.
//
class FunTextWriter
{
public:

  FunTextWriter (std::ostream &out);

  // Write a text representation of FUN.
  //
  void write (Fun *fun);


  // Return a text label for BLOCK.
  //
  std::string block_label (const BB *block);

  // Return a string containing labels for all blocks in BLOCK_LIST,
  // separated by a comma and space.
  //
  std::string block_list_labels (const std::list<BB *> &block_list);


  //
  // The following data members are all public, and reflect common
  // state during writing.
  //


  // Stream we're writing to.
  //
  std::ostream &out;

  // Text writer for insns.
  //
  InsnTextWriter insn_writer;

  // Text writer for blocks.
  //
  BBTextWriter block_writer;
};


#endif // __FUN_TEXT_WRITER_H__
