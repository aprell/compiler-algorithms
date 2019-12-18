// fun-text-reader-parse-fun.cc -- Text-format input of an IR
//	function, function parsing
//
// Copyright © 2019  Miles Bader
//
// Author: Miles Bader <snogglethorpe@gmail.com>
// Created: 2019-11-03
//

#include "fun.h"
#include "reg.h"

#include "cond-branch-insn.h"
#include "nop-insn.h"
#include "calc-insn.h"
#include "copy-insn.h"
#include "fun-arg-insn.h"
#include "fun-result-insn.h"

#include "src-file-input.h"

#include "fun-text-reader.h"


// Parse the contents of a function.
//
void
FunTextReader::parse_fun ()
{
  SrcFileInput &inp = input ();

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
	    inp.error ("No instructions can follow a fun-result instruction");
	}

      // Label (starts a new block)
      //
      if (inp.peek () == '<')
	{
	  BB *prev_block = cur_block;

	  cur_block = read_label ();

	  if (! cur_block->is_empty ())
	    inp.error (std::string ("duplicate label _")
		       + std::to_string (cur_block->num ()));

	  if (prev_block)
	    prev_block->set_fall_through (cur_block);

	  saw_label = true;

	  continue;
	}

      // Register declaration
      //
      if (inp.skip ("reg"))
	{
	  std::string reg_name = inp.read_id ();

	  Reg *&reg = registers[reg_name];
	  if (reg)
	    inp.error (std::string ("Duplicate register declaration \"")
		       + reg_name + "\"");

	  reg = new Reg (reg_name, cur_fun);

	  continue;
	}

      // For everything after this point, a block is expected.
      //
      if (! cur_block)
	inp.error ("Expected label");

      // Branch insn, ends the current block
      //
      if (inp.skip ("goto"))
	{
	  BB *target = read_label ();
	  cur_block->set_fall_through (target);
	  cur_block = 0;

	  continue;
	}

      // Conditional branch insn
      //
      if (inp.skip ("if"))
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
      if (inp.skip ("nop"))
	{
	  new NopInsn (cur_block);
	  continue;
	}

      // Function-argument insn
      //
      if (inp.skip ("fun_arg"))
	{
	  if (saw_label)
	    inp.error
	      ("fun_arg instructions are only valid at the start of a function");

	  unsigned arg_num = inp.read_unsigned ();
	  Reg *arg_reg = read_lvalue_reg ();
	  new FunArgInsn (arg_num, arg_reg, cur_fun->entry_block ());

	  continue;
	}

      //
      // We didn't see any recognized keywords, so assume the input is
      // of the form "REG1, REG2, ... := ..."
      //

      // Vector of result registers for this statement.
      //
      std::vector<Reg *> results = read_lvalue_reg_list ();

      // Source-location after reading the result registers, used for
      // an error message below.
      //
      SrcContext::Loc after_results_src_loc = inp.cur_src_loc ();

      inp.expect (":=");

      // By default, we assume multiple results are not allowed.
      // Instructions that do allow them will turn this off.
      //
      bool multiple_results_ok = false;

      if (inp.skip ('-'))
	{
	  // Unary calc insn

	  Reg *arg = read_rvalue_reg ();

	  new CalcInsn (CalcInsn::Op::NEG, arg, results[0], cur_block);
	}
      else
	{
	  // Copy insn or binary calc insn

	  std::vector<Reg *> args = read_rvalue_reg_list ();

	  inp.skip_whitespace ();

	  if (inp.at_eol ())
	    {
	      if (results.size () != args.size ())
		inp.error ("Number of sources does not match number of destinations",
			   after_results_src_loc);

	      multiple_results_ok = true;
	      new CopyInsn (args, results, cur_block);
	    }
	  else
	    {
	      if (args.size () != 1)
		inp.error ("Multiple values not allowed here");

	      char calc_op_char = inp.read_char ();
	      CalcInsn::Op calc_op;
	      switch (calc_op_char)
		{
		case '+': calc_op = CalcInsn::Op::ADD; break;
		case '-': calc_op = CalcInsn::Op::SUB; break;
		case '*': calc_op = CalcInsn::Op::MUL; break;
		case '/': calc_op = CalcInsn::Op::DIV; break;
		default:
		  inp.error
		    (std::string ("Unknown calculation operation \"")
		     + calc_op_char + "\"");
		}

	      Reg *arg2 = read_rvalue_reg ();

	      new CalcInsn (calc_op, args[0], arg2, results[0], cur_block);
	    }
	}

      if (!multiple_results_ok && results.size () != 1)
	inp.error ("Multiple results not valid for this operation",
		   after_results_src_loc);
    }

  if (cur_block)
    cur_block->set_fall_through (cur_fun->exit_block ());
}
