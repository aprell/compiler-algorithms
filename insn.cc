// insn.cc -- IR instructions
//
// Copyright © 2019  Miles Bader
//
// Author: Miles Bader <snogglethorpe@gmail.com>
// Created: 2019-10-30
//

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
	  result->remove_use (this);
	  result = to;
	  result->add_use (this);
	}
}
