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



// Text writer methods

void
InsnTextWriter::write_cond_branch (Insn *insn)
{
  CondBranchInsn *cond_branch_insn = dynamic_cast<CondBranchInsn *> (insn);

  if (! cond_branch_insn)
    invalid_write_method (insn, "CondBranchInsn");

  std::ostream &out = fun_writer.out;
  BB *target = cond_branch_insn->target ();
  out << "if (";
  Reg *cond = cond_branch_insn->condition ();
  if (cond)
    out << cond->name ();
  else
    out << "0";
  out << ") goto ";
  if (target)
    out << fun_writer.block_label (target);
  else
    out << "-";
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
