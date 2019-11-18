// arith-insn.cc -- IR "no operation" instruction
//
// Copyright © 2019  Miles Bader
//
// Author: Miles Bader <snogglethorpe@gmail.com>
// Created: 2019-11-17
//

#include "arith-insn.h"


// Make a new conditional-branch instruction, which transfers
// control to TARGET if COND contains a non-zero value, optionally
// appending it to block BLOCK.
//
ArithInsn::ArithInsn (Op op, Reg *arg1, Reg *arg2, Reg *result, BB *block)
  : Insn (block, {arg1, arg2}, {result}), _op (op)
{
}
