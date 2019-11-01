#include "bb.h"

#include "cond-branch-insn.h"


// Make a new conditional-branch instruction, which may transfer
// control to TARGET, optionally appending it to block BLOCK.
//
CondBranchInsn::CondBranchInsn (BB *target, BB *block)
  : Insn (block)
{
  if (target)
    set_target (target);
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
