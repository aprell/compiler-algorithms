// fun-text-reader.cc -- Text-format input of an IR function
//
// Copyright © 2019  Miles Bader
//
// Author: Miles Bader <snogglethorpe@gmail.com>
// Created: 2019-11-03
//

#include <stdexcept>

#include "fun.h"
#include "reg.h"
#include "value.h"

#include "cond-branch-insn.h"
#include "nop-insn.h"
#include "calc-insn.h"
#include "copy-insn.h"
#include "fun-arg-insn.h"
#include "fun-result-insn.h"

#include "line-input.h"

#include "prog-text-reader.h"

#include "fun-text-reader.h"


FunTextReader::FunTextReader (ProgTextReader &prog_reader)
  : _prog_reader (prog_reader)
{
}


// Return the text input source we're reading from.
//
LineInput &
FunTextReader::input () const
{
  return _prog_reader.input ();
}


// Read a text representation of a function, and return the new
// function.
//
Fun *
FunTextReader::read ()
{
  // Cannot be called recursively.
  //
  if (cur_fun)
    throw std::runtime_error ("Recursive call to FunTextReader::read");

  Fun *fun = new Fun ();

  cur_fun = fun;
  cur_block = fun->entry_block ();

  parse_fun ();

  cur_fun = 0;
  cur_block = 0;

  // Clear any left over parsing state.
  //
  clear_state ();

  return fun;
}


// Read a register (which must exist).
//
Reg *
FunTextReader::read_lvalue_reg ()
{
  return get_reg (input ().read_id ());
}

// Return the register (which must exist) called NAME
//
Reg *
FunTextReader::get_reg (const std::string &name)
{
  auto reg_it = registers.find (name);
  if (reg_it == registers.end ())
    input ().parse_error
      (std::string ("Unknown register \"") + name + "\"");
  return reg_it->second;
}


// Read either a register (which must exist), or a constant value
// (which will be added to the current function if necessary), and
// return the resulting register.
//
Reg *
FunTextReader::read_rvalue_reg ()
{
  LineInput &inp = input ();

  inp.skip_whitespace ();

  if (inp.is_id_start_char (inp.peek ()))
    {
      return read_lvalue_reg ();
    }
  else
    {
      int int_value = inp.read_int ();

      for (auto existing_value_reg : cur_fun->regs ())
	if (existing_value_reg->is_constant ()
	    && existing_value_reg->value ()->int_value () == int_value)
	  return existing_value_reg;

      return new Reg (new Value (int_value, cur_fun));
    }
}


// Clear any parsing state used while parsing a function.
//
void
FunTextReader::clear_state ()
{
  labeled_blocks.clear ();
}


// Return the block corresponding to the label LABEL.  If no such
// label has yet been encountered, a new block is added and returned.
//
BB *
FunTextReader::label_block (const std::string &label)
{
  BB *&block_ptr = labeled_blocks[label];

  if (! block_ptr)
    block_ptr = new BB (cur_fun);

  return block_ptr;
}
