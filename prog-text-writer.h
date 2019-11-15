// prog-text-writer.h -- Text-format output of an IR program
//
// Copyright © 2019  Miles Bader
//
// Author: Miles Bader <snogglethorpe@gmail.com>
// Created: 2019-11-14
//

#ifndef __PROG_TEXT_WRITER_H__
#define __PROG_TEXT_WRITER_H__

#include <ostream>

#include "fun-text-writer.h"


class Prog;


// A class for outputting text representations of a function.
//
class ProgTextWriter
{
public:

  ProgTextWriter (std::ostream &out);


  // Return the output stream we're writing to.
  //
  std::ostream &out () const { return _out; }


  // Write a text representation of PROG.
  //
  void write (Prog *prog);


private:

  // Stream we're writing to.
  //
  std::ostream &_out;

  // Text writer for insns.
  //
  FunTextWriter _fun_writer;
};


#endif // __PROG_TEXT_WRITER_H__
