// prog-text-reader.h -- Text-format input of an IR program
//
// Copyright © 2019  Miles Bader
//
// Author: Miles Bader <snogglethorpe@gmail.com>
// Created: 2019-11-14
//

#ifndef __PROG_TEXT_READER_H__
#define __PROG_TEXT_READER_H__

#include "fun-text-reader.h"

class Prog;
class SrcFileInput;


// A class for reading a text representations of a program.
//
class ProgTextReader
{
public:

  ProgTextReader (SrcFileInput &inp);


  // Return the text input source we're reading from.
  //
  SrcFileInput &input () const { return _inp; }


  // Read a text representation of a program, and return the new program..
  //
  Prog *read ();


private:

  SrcFileInput &_inp;

  FunTextReader _fun_reader;

  Prog *cur_prog = 0;
};


#endif // __PROG_TEXT_READER_H__
