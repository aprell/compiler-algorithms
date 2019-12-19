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
  virtual bool is_branch_insn () const { return false; }

  // Return true if this instruction has a visible side-effect other
  // than reading from its argument registers and writing to its
  // result registers.
  //
  virtual bool has_side_effect () const { return false; }


  // For each branch target block in this insn which points to FROM,
  // change the target to TO.  TO may be NULL, in which case the
  // target becomes undefined.
  //
  virtual void change_branch_target (BB */* from */, BB */* to */) { }


  // Return a reference to a read-only vector of arguments read by
  // this instruction.  For many insns, this is empty.
  //
  const std::vector<Reg *> &args () const { return _args; }

  // Return a reference to a read-only vector of results written by
  // this instruction.  For many insns, this is empty.
  //
  const std::vector<Reg *> &results () const { return _results; }

  // Change each use of the argument register FROM in this instruction
  // to TO.  TO may be NULL.  This will update FROM and TO accordingly
  // to reflect the new state.
  //
  void change_arg (Reg *from, Reg *to);

  // Change the register used for argument NUM in this instruction to
  // TO.  TO may be NULL.  This will update both TO and the old
  // register used for argument NUM accordingly to reflect the new
  // state.
  //
  void change_arg (unsigned num, Reg *to);

  // Change each define of the result register FROM in this instruction
  // to TO.  TO may be NULL.  This will update FROM and TO accordingly
  // to reflect the new state.
  //
  void change_result (Reg *from, Reg *to);

  // Change the register defined for result NUM in this instruction to
  // TO.  TO may be NULL.  This will update both TO and the old
  // register used for result NUM accordingly to reflect the new
  // state.
  //
  void change_result (unsigned num, Reg *to);


  // Return true if this instruction dominates INSN, meaning that
  // either: (1) they are in the same basic block, and this instruction
  // precedes INSN, or (2) they are in different basic blocks, and
  // this instruction's block dominates INSN's block.
  //
  // An instruction does _not_ dominate itself (because arguments are
  // typically read before results are written).
  //
  bool dominates (Insn *insn) const;


protected:

  Insn (BB *block,
	std::initializer_list<Reg *> init_args = {},
	std::initializer_list<Reg *> init_results = {});

  Insn (BB *block,
	const std::vector<Reg *> &args,
	const std::vector<Reg *> &results);

  // Add an argument.
  //
  void add_arg (Reg *arg);

  // Add a result.
  //
  void add_result (Reg *result);


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
