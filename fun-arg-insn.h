// fun-arg-insn.h -- IR function-argument instruction
//
// Copyright © 2019  Miles Bader
//
// Author: Miles Bader <snogglethorpe@gmail.com>
// Created: 2019-11-18
//

#ifndef __FUN_ARG_INSN_H__
#define __FUN_ARG_INSN_H__


#include "insn.h"


// IR function argument instruction.  This represents a function
// argument.
//
class FunArgInsn : public Insn
{
public:

  // Make a new function-argument instruction, which associates
  // function argument ARG_NUM with the register ARG.
  //
  FunArgInsn (unsigned arg_num, Reg *arg, BB *block = 0)
    : Insn (block, { }, { arg }), _arg_num (arg_num)
  { }


  // Return the number of the function argument which this instruction
  // represents.
  //
  unsigned arg_num () const { return _arg_num; }


private:

  // The number of the function argument which this instruction
  // represents.
  //
  unsigned _arg_num;
};


#endif // __FUN_ARG_INSN_H__
