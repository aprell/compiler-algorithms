// calc-insn.h -- IR calculation instruction
//
// Copyright © 2019  Miles Bader
//
// Author: Miles Bader <snogglethorpe@gmail.com>
// Created: 2019-11-18
//

#ifndef __CALC_INSN_H__
#define __CALC_INSN_H__


#include "insn.h"


// IR calculation instruction.  This does some sort of calculation on
// a set of arguments, yielding a result.
//
class CalcInsn : public Insn
{
public:

  enum class Op { NONE, ADD, SUB, MUL, DIV };


  // Make a new calculation instruction, which performs the calculation
  // OP on arguments ARG1 and ARG2, storing the result in RESULT.
  // If BLOCK is non-NULL, the instruction is appended to it.
  //
  CalcInsn (Op op, Reg *arg1, Reg *arg2, Reg *result, BB *block = 0);


  // Return the operation for this instruction.
  //
  Op op () const { return _op; }


private:

  Op _op;
};


#endif // __CALC_INSN_H__
