// phi-fun-insn.h -- IR phi-function instruction
//
// Copyright © 2019  Miles Bader
//
// Author: Miles Bader <snogglethorpe@gmail.com>
// Created: 2019-11-25
//

#ifndef __PHI_FUN_INSN_H__
#define __PHI_FUN_INSN_H__

#include <list>

#include "insn.h"


class PhiFunInpInsn;


// IR phi-function instruction.  This is a "magic" instruction used in
// SSA-form, which can only occur at the beginning of a basic block,
// and which returns a value depending on which predecessor block
// control comes from.
//
// A phi-function instruction only contains the output of the
// phi-function; the inputs are contained within associated
// phi-function-input instructions (class PhiFunInpInsn).
//
class PhiFunInsn : public Insn
{
public:

  // Make a new phi-function instruction for the register REG.
  //
  // If BLOCK is non-NULL, the instruction is prepended to it
  // (phi-functions must always come at the beginning of a block).
  //
  PhiFunInsn (Reg *reg, BB *block = 0);

  ~PhiFunInsn ();


  // Return a reference to a read-only list containing inputs for this phi-function.
  //
  const std::list<PhiFunInpInsn *> &inputs () const { return _inputs; }

  // Add the phi-function input INPUT to this phi-function.
  // Does not modify INPUT (so it won't make INPUT point to this phi-function).
  //
  void add_input (PhiFunInpInsn *input);

  // Remove the phi-function input INPUT from this phi-function.
  // Does not modify INPUT (so if INPUT points to this phi-function,
  // that won't change).
  //
  void remove_input (PhiFunInpInsn *input);


private:

  // A list of the inputs for this phi-function.
  //
  std::list<PhiFunInpInsn *> _inputs;
};


#endif // __PHI_FUN_INSN_H__
