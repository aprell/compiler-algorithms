// insn.cc -- IR instructions
//
// Copyright © 2019  Miles Bader
//
// Author: Miles Bader <snogglethorpe@gmail.com>
// Created: 2019-10-30
//

#include "check-assertion.h"

#include "bb.h"
#include "reg.h"

#include "insn.h"


Insn::Insn (BB *block,
	    std::initializer_list<Reg *> init_args,
	    std::initializer_list<Reg *> init_results)
  : _args (init_args), _results (init_results)
{
  for (auto arg : _args)
    if (arg)
      arg->add_use (this);
  for (auto result : _results)
    if (result)
      result->add_def (this);

  if (block)
    block->add_insn (this);
}

Insn::~Insn ()
{
  if (_block)
    _block->remove_insn (this);

  for (auto arg : _args)
    if (arg)
      arg->remove_use (this);
  for (auto result : _results)
    if (result)
      result->remove_def (this);
}


// Add an argument.
//
void
Insn::add_arg (Reg *arg)
{
  _args.push_back (arg);

  if (arg)
    arg->add_use (this);
}


// Change each use of FROM in this instruction to TO.  TO may be NULL.
// This will update FROM and TO accordingly to reflect the new state.
//
void
Insn::change_arg (Reg *from, Reg *to)
{
  if (from != to)
    for (auto &arg : _args)
      if (arg == from)
	{
	  arg->remove_use (this);
	  arg = to;
	  arg->add_use (this);
	}
}

// Change argument NUM in this instruction to TO.  TO may be NULL.
// This will update both TO and the old argument NUM accordingly to
// reflect the new state.
//
void
Insn::change_arg (unsigned num, Reg *to)
{
  check_assertion (num < _args.size (),
		   "Invalid argument index in Insn::change_arg");

  Reg *from = _args[num];
  if (from != to)
    {
      from->remove_use (this);
      _args[num] = to;
      to->add_use (this);
    }
}

// Change each define of FROM in this instruction to TO.  TO may be NULL.
// This will update FROM and TO accordingly to reflect the new state.
//
void
Insn::change_result (Reg *from, Reg *to)
{
  if (from != to)
    for (auto &result : _results)
      if (result == from)
	{
	  result->remove_def (this);
	  result = to;
	  result->add_def (this);
	}
}

// Change result NUM in this instruction to TO.  TO may be NULL.
// This will update both TO and the old result NUM accordingly to
// reflect the new state.
//
void
Insn::change_result (unsigned num, Reg *to)
{
  check_assertion (num < _results.size (),
		   "Invalid result index in Insn::change_result");

  Reg *from = _results[num];
  if (from != to)
    {
      from->remove_def (this);
      _results[num] = to;
      to->add_def (this);
    }
}


// Return true if this instruction dominates INSN, meaning that
// either: (1) they are in the same basic block, and this instruction
// precedes INSN, or (2) they are in different basic blocks, and
// this instruction's block dominates INSN's block.
//
// An instruction does _not_ dominate itself (because arguments are
// typically read before results are written).
//
bool
Insn::dominates (Insn *insn) const
{
  // An instruction does not dominate itself.
  //
  if (this == insn)
    return false;

  if (_block == insn->_block)
    {
      // In same block, check to see that this instruction comes
      // first.

      for (auto search : _block->insns ())
	if (search == this)
	  return true;		// this insn is first
	else if (search == insn)
	  return false;		// INSN is first

      check_assertion_failure ("Instruction not found in Insn::dominates");
    }
  else
    {
      // Check to see if our block dominates INSN's block.
      //
      return _block->dominates (insn->_block, true);
    }
}
