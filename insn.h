// insn.h -- IR instructions
//
// Copyright © 2019  Miles Bader
//
// Author: Miles Bader <snogglethorpe@gmail.com>
// Created: 2019-10-30
//

#ifndef __INSN_H__
#define __INSN_H__

#include <vector>
#include <initializer_list>


class BB;
class Reg;


// IR instruction, representing a single operation in a flow graph.
//
class Insn
{
public:

  virtual ~Insn ();


  // Return the block this instruction is in.
  //
  BB *block () const { return _block; }

  // Do instruction-specific setup after this instruction has been
  // added to block BLOCK.  At the point this is called, this
  // instruction is in BLOCK's instruction list, but nothing else has
  // been done.
  //
  virtual void set_block (BB *block) { _block = block; }


  // Return true if this is a branch instruction, that is, if it may
  // affect control-flow.
  //
  virtual bool is_branch_insn () { return false; }

  // For each branch target block in this insn which points to FROM,
  // change the target to TO.  TO may be NULL, in which case the
  // target becomes undefined.
  //
  virtual void change_branch_target (BB */* from */, BB */* to */) { }


  // Return a vector of arguments read by this.  For many insns, this is empty.
  //
  const std::vector<Reg *> &args () const { return _args; }

  // Return a vector of results written by this insn.  For many insns, this is empty.
  //
  const std::vector<Reg *> &results () const { return _results; }

  // Change each use of FROM in this instruction to TO.  TO may be NULL.
  // This will update FROM and TO accordingly to reflect the new state.
  //
  void change_arg (Reg *from, Reg *to);

  // Change each define of FROM in this instruction to TO.  TO may be NULL.
  // This will update FROM and TO accordingly to reflect the new state.
  //
  void change_result (Reg *from, Reg *to);


protected:

  Insn (BB *block,
	std::initializer_list<Reg *> init_args = {},
	std::initializer_list<Reg *> init_results = {});


private:

  // Arguments to, and results from, this calculation insn.
  //
  std::vector<Reg *> _args;
  std::vector<Reg *> _results;


  // The block this instruction is in.
  //
  BB *_block = 0;
};



#endif // __INSN_H__
