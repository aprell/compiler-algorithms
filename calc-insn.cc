// calc-insn.cc -- IR calculation instruction
//
// Copyright © 2019  Miles Bader
//
// Author: Miles Bader <snogglethorpe@gmail.com>
// Created: 2019-11-18
//

#include "calc-insn.h"


// Make a new calculation instruction, which performs the calculation
// OP on arguments ARG1 and ARG2, storing the result in RESULT.
// If BLOCK is non-NULL, the instruction is appended to it.
//
CalcInsn::CalcInsn (Op op, Reg *arg1, Reg *arg2, Reg *result, BB *block)
  : Insn (block, {arg1, arg2}, {result}), _op (op)
{
}
