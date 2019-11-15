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
#include <list>

#include "insn-text-writer.h"
#include "bb-text-writer.h"


class ProgTextWriter;

class Fun;


// A class for outputting text representations of a function.
//
class FunTextWriter
{
public:

  FunTextWriter (ProgTextWriter &prog_writer);


  // Return the output stream we're writing to.
  //
  std::ostream &output_stream ();


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


  // Text writer for the program we're associated with.
  //
  ProgTextWriter &prog_writer;


  // Text writer for insns.
  //
  InsnTextWriter insn_writer;

  // Text writer for blocks.
  //
  BBTextWriter block_writer;
};


#endif // __FUN_TEXT_WRITER_H__
