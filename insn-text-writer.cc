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
#include "arith-insn.h"

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
InsnTextWriter::write_arith (Insn *insn)
{
  if (ArithInsn *arith_insn = dynamic_cast<ArithInsn *> (insn))
    {
      std::ostream &out = fun_writer.output_stream ();

      const std::vector<Reg *> &args = arith_insn->args ();
      const std::vector<Reg *> &results = arith_insn->results ();

      std::string op_name;
      switch (arith_insn->op ())
	{
	case ArithInsn::Op::ADD: op_name = "+"; break;
	case ArithInsn::Op::SUB: op_name = "-"; break;
	case ArithInsn::Op::MUL: op_name = "*"; break;
	case ArithInsn::Op::DIV: op_name = "/"; break;
	default:
	  op_name
	    = "?("
	    + std::to_string (static_cast<int> (arith_insn->op ()))
	    + ")";
	}

      out << reg_name (results[0]) << " := "
	  << reg_name (args[0])
	  << ' ' << op_name << ' '
	  << reg_name (args[1]);
    }
  else
    invalid_write_method (insn, "ArithInsn");
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
  write_meths[typeid (ArithInsn)] = &InsnTextWriter::write_arith;
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
