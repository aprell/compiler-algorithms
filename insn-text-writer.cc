// insn-text-writer.cc -- Text-format output of an IR instruction
//
// Copyright © 2019  Miles Bader
//
// Author: Miles Bader <snogglethorpe@gmail.com>
// Created: 2019-11-02
//

#include <stdexcept>
#include <string>
#include <ostream>

#include "bb.h"
#include "reg.h"

#include "cond-branch-insn.h"
#include "nop-insn.h"
#include "calc-insn.h"
#include "fun-arg-insn.h"

#include "fun-text-writer.h"

#include "insn-text-writer.h"


InsnTextWriter::InsnTextWriter (FunTextWriter &_fun_writer)
  : fun_writer (_fun_writer) 
{
  setup_write_meths ();
}


// Write a text representation of INSN.
//
void
InsnTextWriter::write (Insn *insn)
{
  auto write_meth_entry = write_meths.find (typeid (*insn));
  if (write_meth_entry != write_meths.end ())
    (this->*(write_meth_entry->second)) (insn);
  else
    invalid_write_method (insn, 0);
}


// Return a string representation of the register REG
//
std::string
InsnTextWriter::reg_name (Reg *reg) const
{
  if (reg)
    return reg->name ();
  else
    return "?";
}



// Text writer methods

void
InsnTextWriter::write_cond_branch (Insn *insn)
{
  CondBranchInsn *cond_branch_insn = dynamic_cast<CondBranchInsn *> (insn);

  if (! cond_branch_insn)
    invalid_write_method (insn, "CondBranchInsn");

  std::ostream &out = fun_writer.output_stream ();
  BB *target = cond_branch_insn->target ();
  out << "if (" << reg_name (cond_branch_insn->condition ()) << ") goto ";
  if (target)
    out << fun_writer.block_label (target);
  else
    out << "-";
}

void
InsnTextWriter::write_nop (Insn *)
{
  std::ostream &out = fun_writer.output_stream ();
  out << "nop";
}

void
InsnTextWriter::write_calc (Insn *insn)
{
  if (CalcInsn *calc_insn = dynamic_cast<CalcInsn *> (insn))
    {
      std::ostream &out = fun_writer.output_stream ();

      const std::vector<Reg *> &args = calc_insn->args ();
      const std::vector<Reg *> &results = calc_insn->results ();

      std::string op_name;
      switch (calc_insn->op ())
	{
	case CalcInsn::Op::ADD: op_name = "+"; break;
	case CalcInsn::Op::SUB: op_name = "-"; break;
	case CalcInsn::Op::MUL: op_name = "*"; break;
	case CalcInsn::Op::DIV: op_name = "/"; break;
	default:
	  op_name
	    = "?("
	    + std::to_string (static_cast<int> (calc_insn->op ()))
	    + ")";
	}

      out << reg_name (results[0]) << " := "
	  << reg_name (args[0])
	  << ' ' << op_name << ' '
	  << reg_name (args[1]);
    }
  else
    invalid_write_method (insn, "CalcInsn");
}

void
InsnTextWriter::write_fun_arg (Insn *insn)
{
  if (FunArgInsn *fun_arg_insn = dynamic_cast<FunArgInsn *> (insn))
    {
      std::ostream &out = fun_writer.output_stream ();

      const std::vector<Reg *> &results = fun_arg_insn->results ();

      out << "fun_arg "
	  << fun_arg_insn->arg_num ()
	  << ' '
	  << reg_name (results[0]);
    }
  else
    invalid_write_method (insn, "FunArgInsn");
}



// Text writer method dispatch machinery.


std::unordered_map<std::type_index, InsnTextWriter::WriteMeth> InsnTextWriter::write_meths;


// Populate the WRITE_METHS data member.
//
void
InsnTextWriter::setup_write_meths ()
{
  // If we've already done this setup, just return.
  //
  if (! write_meths.empty ())
    return;

  write_meths[typeid (CondBranchInsn)] = &InsnTextWriter::write_cond_branch;
  write_meths[typeid (NopInsn)] = &InsnTextWriter::write_nop;
  write_meths[typeid (CalcInsn)] = &InsnTextWriter::write_calc;
  write_meths[typeid (FunArgInsn)] = &InsnTextWriter::write_fun_arg;
}


// Called when there's a mixup and write method that expected an insn
// of type WANTED is called on the insn INSN which is not of that
// type.  Raises an exception.
//
void
InsnTextWriter::invalid_write_method (Insn *insn, const char *wanted)
{
  std::string msg;
  if (wanted)
    {
      msg += "Invalid InsnTextWriter method called; wanted an insn of type ";
      msg += wanted;
      msg += " but got one of type ";
    }
  else
    {
      msg += "No InsnTextWriter method for insn of type ";
    }
  msg += typeid (*insn).name ();
  throw std::runtime_error (msg);
}
