#include <stdexcept>

#include "fun.h"
#include "reg.h"

#include "cond-branch-insn.h"

#include "text-reader-inp.h"

#include "fun-text-reader.h"


FunTextReader::FunTextReader (TextReaderInp &_inp)
  : insn_reader (*this), block_reader (*this), inp (_inp)
{
}


// Read a text representation of a function, and return the new function..
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
  parse_fun ();
  cur_fun = 0;

  // Clear any left over parsing state.
  //
  clear_state ();

  return fun;
}


// Parse the contents of a function.
//
void
FunTextReader::parse_fun ()
{
  inp.read_new_line ();
  inp.expect ('{');

  bool first_block = true;

  while (inp.read_new_line ())
    {
      if (inp.skip ('}'))
	break;

      // Comment line
      //
      if (inp.skip ('#'))
	continue;

      // Label (starts a new block)
      //
      if (inp.skip_eol (':'))
	{
	  BB *prev_block = cur_block;

	  cur_block = read_label ();

	  if (! cur_block->is_empty ())
	    inp.parse_error (std::string ("duplicate label _") + std::to_string (cur_block->num ()));

	  if (prev_block)
	    prev_block->set_fall_through (cur_block);

	  if (first_block)
	    {
	      cur_fun->set_entry_block (cur_block);
	      first_block = false;
	    }

	  continue;
	}

      // All other command forms start with an ID, which may be a
      // register name or a command name.
      //
      std::string id = inp.read_id ();

      // Register declaration
      //
      if (id == "reg" && inp.peek () != ':')
	{
	  std::string reg_name = inp.read_id ();

	  Reg *&reg = registers[reg_name];
	  if (reg)
	    inp.parse_error (std::string ("Duplicate register declaration \"") + id + "\"");
	  reg = new Reg (reg_name);

	  cur_fun->add_reg (reg);

	  continue;
	}

      // For everything after this point, a block is expected.
      //
      if (! cur_block)
	inp.parse_error ("Expected label");

      // Assignment, of the form "REG := ..."
      //
      if (inp.peek () == ':')
	{
	  inp.expect ('=');

	  continue;
	}

      // Branch insn, ends the current block
      //
      if (id == "goto")
	{
	  BB *target = read_label ();
	  cur_block->set_fall_through (target);
	  cur_block = 0;

	  continue;
	}

      // Conditional branch insn
      //
      if (id == "if")
	{
	  inp.expect ('(');
	  Reg *cond = read_reg ();
	  inp.expect (')');
	  inp.expect ("goto");

	  BB *target = read_label ();

	  new CondBranchInsn (cond, target, cur_block);

	  continue;
	}
    }

  // If control continues to the end of the function, add an exit
  // block.
  //
  if (cur_block)
    {
      // If the current block isn't suitable for use as an exit block,
      // add a new block following it.
      //
      if (! cur_block->is_empty () || ! cur_block->successors ().empty ())
	{
	  BB *last_user_block = cur_block;
	  cur_block = new BB (cur_fun);
	  last_user_block->set_fall_through (cur_block);
	}

      cur_fun->set_exit_block (cur_block);
    }
}


// Read a register (which must exist) .
//
Reg *
FunTextReader::read_reg ()
{
  std::string reg_name = inp.read_id ();

  auto reg_it = registers.find (reg_name);
  if (reg_it == registers.end ())
    inp.parse_error (std::string ("Unknown register \"") + reg_name + "\"");

  return reg_it->second;
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
