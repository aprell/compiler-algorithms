// phi-fun-inp-insn.cc -- IR phi-function input instruction
//
// Copyright © 2019  Miles Bader
//
// Author: Miles Bader <snogglethorpe@gmail.com>
// Created: 2019-11-25
//

#include "bb.h"

#include "phi-fun-insn.h"

#include "phi-fun-inp-insn.h"


// Make a new phi-function input instruction for the phi-function
// instruction PHI_FUN, using ARG as the input to the phi function..
//
// If BLOCK is non-NULL, the instruction is added to, before any
// branch insn.
//
PhiFunInpInsn::PhiFunInpInsn (PhiFunInsn *phi_fun, Reg *arg, BB *block)
  : Insn (0, { arg }, { }), _phi_fun (phi_fun)
{
  if (phi_fun)
    phi_fun->add_input (this);

  if (block)
    block->add_insn_before_branch (this);
}

PhiFunInpInsn::~PhiFunInpInsn ()
{
  if (PhiFunInsn *phi_fun = _phi_fun)
    {
      _phi_fun = 0;
      phi_fun->remove_input (this);
    }
}
