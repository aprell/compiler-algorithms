// phi-fun-insn.cc -- IR phi-function instruction
//
// Copyright © 2019  Miles Bader
//
// Author: Miles Bader <snogglethorpe@gmail.com>
// Created: 2019-11-25
//

#include "bb.h"
#include "reg.h"

#include "phi-fun-inp-insn.h"

#include "phi-fun-insn.h"


// Make a new phi-function instruction for the register REG.
//
// If BLOCK is non-NULL, the instruction is prepended to it
// (phi-functions must always come at the beginning of a block).
//
PhiFunInsn::PhiFunInsn (Reg *reg, BB *block)
  : Insn (0, { }, { reg })
{
  if (block)
    block->prepend_insn (this);
}

PhiFunInsn::~PhiFunInsn ()
{
  for (auto input : _inputs)
    input->set_phi_fun (0);

  _inputs.clear ();
}


// Add the phi-function input INPUT to this phi-function.
// Does not modify INPUT (so it won't make INPUT point to this phi-function).
//
void
PhiFunInsn::add_input (PhiFunInpInsn *input)
{
  _inputs.push_back (input);
}

// Remove the phi-function input INPUT from this phi-function.
// Does not modify INPUT (so if INPUT points to this phi-function,
// that won't change).
//
void
PhiFunInsn::remove_input (PhiFunInpInsn *input)
{
  _inputs.remove (input);
}
