// prog-text-writer.cc -- Text-format output of an IR program
//
// Copyright © 2019  Miles Bader
//
// Author: Miles Bader <snogglethorpe@gmail.com>
// Created: 2019-11-14
//

#include "prog.h"

#include "prog-text-writer.h"


ProgTextWriter::ProgTextWriter (std::ostream &out)
  : _out (out), _fun_writer (*this)
{
}


// Write a text representation of PROG.
//
void
ProgTextWriter::write (Prog *prog)
{
  for (auto [name, fun] : prog->functions ())
    {
      _out << "fun " << name << '\n';
      _fun_writer.write (fun);
      _out << '\n';
    }
}
