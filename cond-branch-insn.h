#ifndef __COND_BRANCH_INSN_H__
#define __COND_BRANCH_INSN_H__


#include "insn.h"


class CondBranchInsn : public Insn
{
public:

  // Make a new conditional-branch instruction, which may transfer
  // control to TARGET, optionally appending it to block BLOCK.
  //
  CondBranchInsn (BB *target = 0, BB *block = 0);

  // Do instruction-specific setup after this instruction has been
  // added to block BLOCK.  At the point this is called, this
  // instruction is in BLOCK's instruction list, but nothing else has
  // been done.
  //
  virtual void set_block (BB *block);

  // Return true if this is a branch instruction, that is, if it may
  // affect control-flow.
  //
  virtual bool is_branch_insn () { return true; }

  // Return the block to which this conditional branch instruction may
  // transfer control [if -some- condition is true].
  //
  BB *target () const { return _target; }

  // Set the block to which this conditional branch instruction may
  // transfer control [if -some- condition is true].
  //
  void set_target (BB *target);


private:

  // Block to which this conditional branch instruction may transfer
  // control [if -some- condition is true].
  //
  BB *_target = 0;
};


#endif // __COND_BRANCH_INSN_H__
