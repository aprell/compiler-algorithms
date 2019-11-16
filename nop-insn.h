// nop-insn.h -- IR "no operation" instruction
//
// Copyright © 2019  Miles Bader
//
// Author: Miles Bader <snogglethorpe@gmail.com>
// Created: 2019-10-31
//

#ifndef __NOP_INSN_H__
#define __NOP_INSN_H__


#include "insn.h"


// IR "no operation" instruction.  This is an opaque instruction which
// does nothing, but cannot be removed.
//
class NopInsn : public Insn
{
public:

  // Make a new conditional-branch instruction, which transfers
  // control to TARGET if COND contains a non-zero value, optionally
  // appending it to block BLOCK.p
  //
  NopInsn (BB *block = 0) : Insn (block) { }
};


#endif // __NOP_INSN_H__
