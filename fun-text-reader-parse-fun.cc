// fun-text-reader-parse-fun.cc -- Text-format input of an IR
//	function, function parsing
//
// Copyright © 2019  Miles Bader
//
// Author: Miles Bader <snogglethorpe@gmail.com>
// Created: 2019-12-01
//

#include "fun.h"
#include "reg.h"

#include "cond-branch-insn.h"
#include "nop-insn.h"
#include "calc-insn.h"
#include "copy-insn.h"
#include "fun-arg-insn.h"
#include "fun-result-insn.h"

#include "line-input.h"

#include "fun-text-reader.h"


// Parse the contents of a function.
//
void
FunTextReader::parse_fun ()
{
  LineInput &inp = input ();

  inp.read_new_line ();
  inp.expect ('{');

  // True if we've seen at least one label, meaning we're past the
  // function beginning into the body.
  //
  bool saw_label = false;

  // True if we've seen a function-result instruction, after which no
  // other instructions are allowed.
  //
  bool saw_fun_result = false;

  while (inp.read_new_line ())
    {
      if (inp.skip ('}'))
	break;

      // Comment line
      //
      if (inp.skip ('#') || inp.at_eol ())
	continue;

      // A "function result" insn, which must come at the end of the
      // function.
      //
      if (inp.skip ("fun_result"))
	{
	  unsigned result_num = inp.read_unsigned ();
	  Reg *result_reg = read_lvalue_reg ();
	  new FunResultInsn (result_num,
			     result_reg, cur_fun->exit_block ());

	  saw_fun_result = true;

	  continue;
	}
      else
	{
	  // Anything _except_ a function result insn must not follow
	  // one.

	  if (saw_fun_result)
	    inp.parse_error
	      ("No instructions can follow a fun-result instruction");
	}

      // Label (starts a new block)
      //
      if (inp.skip_eol (':'))
	{
	  BB *prev_block = cur_block;

	  cur_block = read_label ();

	  if (! cur_block->is_empty ())
	    inp.parse_error (std::string ("duplicate label _")
			     + std::to_string (cur_block->num ()));

	  if (prev_block)
	    prev_block->set_fall_through (cur_block);

	  saw_label = true;

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
	    inp.parse_error
	      (std::string ("Duplicate register declaration \"")
	       + id + "\"");

	  reg = new Reg (reg_name, cur_fun);

	  continue;
	}

      // For everything after this point, a block is expected.
      //
      if (! cur_block)
	inp.parse_error ("Expected label");

      inp.skip_whitespace ();

      // Assignment, of the form "REG := ..."
      //
      if (inp.peek () == ':')
	{
	  Reg *result = get_reg (id);

	  inp.skip (':');
	  inp.expect ('=');

	  inp.skip_whitespace ();

	  if (inp.skip ('-'))
	    {
	      // Unary calc insn

	      Reg *arg = read_rvalue_reg ();

	      new CalcInsn (CalcInsn::Op::NEG, arg, result, cur_block);
	    }
	  else
	    {
	      // Copy insn or binary calc insn

	      Reg *arg1 = read_rvalue_reg ();

	      inp.skip_whitespace ();

	      if (inp.at_eol ())
		{
		  new CopyInsn (arg1, result, cur_block);
		}
	      else
		{
		  char calc_op_char = inp.read_char ();
		  CalcInsn::Op calc_op;
		  switch (calc_op_char)
		    {
		    case '+': calc_op = CalcInsn::Op::ADD; break;
		    case '-': calc_op = CalcInsn::Op::SUB; break;
		    case '*': calc_op = CalcInsn::Op::MUL; break;
		    case '/': calc_op = CalcInsn::Op::DIV; break;
		    default:
		      inp.parse_error
			(std::string ("Unknown calculation operation \"")
			 + calc_op_char + "\"");
		    }

		  Reg *arg2 = read_rvalue_reg ();

		  new CalcInsn (calc_op, arg1, arg2, result, cur_block);
		}
	    }

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
	  Reg *cond = read_rvalue_reg ();
	  inp.expect (')');
	  inp.expect ("goto");

	  BB *target = read_label ();

	  new CondBranchInsn (cond, target, cur_block);

	  continue;
	}

      // No-operation insn
      //
      if (id == "nop")
	{
	  new NopInsn (cur_block);
	  continue;
	}

      // Function-argument insn
      //
      if (id == "fun_arg")
	{
	  if (saw_label)
	    inp.parse_error
	      ("fun_arg instructions are only valid at the start of a function");

	  unsigned arg_num = inp.read_unsigned ();
	  Reg *arg_reg = read_lvalue_reg ();
	  new FunArgInsn (arg_num, arg_reg, cur_fun->entry_block ());

	  continue;
	}

      inp.parse_error ("Unknown instruction");
    }

  if (cur_block)
    cur_block->set_fall_through (cur_fun->exit_block ());
}
