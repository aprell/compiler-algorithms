// fun-result-insn.h -- IR function-result instruction
//
// Copyright © 2019  Miles Bader
//
// Author: Miles Bader <snogglethorpe@gmail.com>
// Created: 2019-11-18
//

#ifndef __FUN_RESULT_INSN_H__
#define __FUN_RESULT_INSN_H__


#include "insn.h"


// IR function result instruction.  This represents a function
// result.
//
class FunResultInsn : public Insn
{
public:

  // Make a new function-result instruction, which stores the register
  // RESULT into the function result RESULT_NUM.
  //
  FunResultInsn (unsigned result_num, Reg *result, BB *block = 0)
    : Insn (block, { result }, { }), _result_num (result_num)
  { }


  // Return true if this instruction has a visible side-effect other
  // than reading from its argument registers and writing to its
  // result registers.
  //
  virtual bool has_side_effect () const { return true; }


  // Return the number of the function result which this instruction
  // represents.
  //
  unsigned result_num () const { return _result_num; }


private:

  // The number of the function result which this instruction
  // represents.
  //
  unsigned _result_num;
};


#endif // __FUN_RESULT_INSN_H__
