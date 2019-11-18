// arith-insn.h -- IR "no operation" instruction
//
// Copyright © 2019  Miles Bader
//
// Author: Miles Bader <snogglethorpe@gmail.com>
// Created: 2019-11-17
//

#ifndef __ARITH_INSN_H__
#define __ARITH_INSN_H__


#include "insn.h"


// IR arithmetic instruction.  This does some sort of arithmetic
// calculation on a set of arguments, yielding a single result.
//
class ArithInsn : public Insn
{
public:

  enum class Op { NONE, ADD, SUB, MUL, DIV };


  // Make a new conditional-branch instruction, which transfers
  // control to TARGET if COND contains a non-zero value, optionally
  // appending it to block BLOCK.
  //
  ArithInsn (Op op, Reg *arg1, Reg *arg2, Reg *result, BB *block = 0);


  // Return the operation for this instruction.
  //
  Op op () const { return _op; }


private:

  Op _op;
};


#endif // __ARITH_INSN_H__
