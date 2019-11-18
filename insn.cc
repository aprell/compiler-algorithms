// insn.cc -- IR instructions
//
// Copyright © 2019  Miles Bader
//
// Author: Miles Bader <snogglethorpe@gmail.com>
// Created: 2019-10-30
//

#include "bb.h"

#include "insn.h"


Insn::Insn (BB *block)
{
  if (block)
    block->add_insn (this);
}

Insn::~Insn ()
{
  if (_block)
    _block->remove_insn (this);
}


// This is used as a default value for empty result vectors.
//
const std::vector<Reg *> Insn::_null_vec;
