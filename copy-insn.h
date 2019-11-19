// copy-insn.h -- IR copy instruction
//
// Copyright © 2019  Miles Bader
//
// Author: Miles Bader <snogglethorpe@gmail.com>
// Created: 2019-11-18
//

#ifndef __COPY_INSN_H__
#define __COPY_INSN_H__

#include "insn.h"


// IR copy instruction.
//
class CopyInsn : public Insn
{
public:

  // Make a new copy instruction, which copies FROM to TO
  // If BLOCK is non-NULL, the instruction is appended to it.
  //
  CopyInsn (Reg *from, Reg *to, BB *block = 0)
    : Insn (block, { from }, { to })
  { }
};


#endif // __COPY_INSN_H__
