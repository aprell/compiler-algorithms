// phi-fun-inp-insn.h -- IR phi-function input instruction
//
// Copyright © 2019  Miles Bader
//
// Author: Miles Bader <snogglethorpe@gmail.com>
// Created: 2019-11-25
//

#ifndef __PHI_FUN_INP_INSN_H__
#define __PHI_FUN_INP_INSN_H__

#include "insn.h"


class PhiFunInsn;


// IR phi-function input instruction.  This is a "magic" instruction
// used in SSA-form, which can only occur at the end of a basic block
// (before any branch insn), which is associated with a phi-function
// instruction (class PhiFunInsn) in a successor block, and contains
// the phi-function input value used for control transfers from this
// block to that successor.
//
class PhiFunInpInsn : public Insn
{
public:

  // Make a new phi-function input instruction for the phi-function
  // instruction PHI_FUN, using ARG as the input to the phi function..
  //
  // If BLOCK is non-NULL, the instruction is added to, before any
  // branch insn.
  //
  PhiFunInpInsn (PhiFunInsn *phi_fun, Reg *arg, BB *block = 0);

  ~PhiFunInpInsn ();


  // Return the phi-function instruction this input is for.
  //
  PhiFunInsn *phi_fun () const { return _phi_fun; }

  // Set this input's phi-function to PHI_FUN.
  // Does not modify PHI_FUN.
  //
  void set_phi_fun (PhiFunInsn *phi_fun) { _phi_fun = phi_fun; }


private:

  // The phi-function instruction this input is for.
  //
  PhiFunInsn *_phi_fun;
};


#endif // __PHI_FUN_INP_INSN_H__
