// insn-text-reader.h -- Text-format input of an IR instruction
//
// Copyright © 2019  Miles Bader
//
// Author: Miles Bader <snogglethorpe@gmail.com>
// Created: 2019-11-03
//

#ifndef __INSN_TEXT_READER_H__
#define __INSN_TEXT_READER_H__


class Insn;
class FunTextReader;


// A class for outputting text representations of isns.
//
class InsnTextReader
{
public:

  InsnTextReader (FunTextReader &fun_reader);

  // Read a text representation of INSN.
  //
  void read (Insn *insn);


private:

  // Text reader for the function we're associated with.
  //
  FunTextReader &fun_reader;
};


#endif // __INSN_TEXT_READER_H__
