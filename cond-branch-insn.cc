// cond-branch-insn.cc -- IR conditional branch instructions
//
// Copyright © 2019  Miles Bader
//
// Author: Miles Bader <snogglethorpe@gmail.com>
// Created: 2019-10-31
//

#include "check-assertion.h"

#include "bb.h"
#include "reg.h"

#include "cond-branch-insn.h"


// Make a new conditional-branch instruction, which transfers control
// to TARGET if COND contains a non-zero value, optionally appending
// it to block BLOCK.
//
CondBranchInsn::CondBranchInsn (Reg *cond, BB *target, BB *block)
  : Insn (block), _cond_vec ({ cond })
{
  if (target)
    set_target (target);

  if (cond)
    cond->add_use (this);
}

CondBranchInsn::~CondBranchInsn ()
{
  // We need to override the destructor to make sure that we can
  // properly remove this insn from its block.  Insn::~Insn also
  // removes the insn from the block, but by the time it's called, the
  // vtable has been changed so that our overrides aren't called, and .

  BB *bb = block ();
  if (bb)
    bb->remove_insn (this);

  Reg *cond = condition ();
  if (cond)
    cond->remove_use (this);
}

// Do instruction-specific setup after this instruction has been
// added to block BLOCK.  At the point this is called, this
// instruction is in BLOCK's instruction list, but nothing else has
// been done.
//
void
CondBranchInsn::set_block (BB *bb)
{
  // If was previously in some other block, remove the effects of
  // that.
  //
  BB *old_block = block ();
  if (old_block && _target)
    old_block->remove_successor (_target);

  // Add the new control flow edge.
  //
  if (bb && _target)
    bb->add_successor (_target);

  Insn::set_block (bb);
}

// For each branch target block in this insn which points to FROM,
// change the target to TO.  TO may be NULL, in which case the target
// becomes undefined.
//
void
CondBranchInsn::change_branch_target (BB *from, BB *to)
{
  if (from == _target)
    set_target (to);
}


// Set the block to which this conditional branch instruction may
// transfer control [if -some- condition is true].
//
void
CondBranchInsn::set_target (BB *target)
{
  // Only need to do control-graph changes if we're actually in a
  // block.
  //
  BB *bb = block ();
  if (bb)
    {
      // Remove old target.
      //
      if (_target)
	bb->remove_successor (_target);

      // Add new target.
      //
      if (target)
	bb->add_successor (target);

      _target = target;
    }
}
