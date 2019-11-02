#include <stdexcept>
#include <string>
#include <iostream>

#include "bb.h"
#include "reg.h"

#include "cond-branch-insn.h"

#include "flow-graph-io.h"

#include "insn-io.h"


InsnDumper::InsnDumper (FlowGraphDumper &_flow_graph_dumper)
  : flow_graph_dumper (_flow_graph_dumper) 
{
  setup_dump_meths ();
}


// Write a text representation of INSN.
//
void
InsnDumper::dump (Insn *insn)
{
  auto dump_meth_entry = dump_meths.find (typeid (*insn));
  if (dump_meth_entry != dump_meths.end ())
    (this->*(dump_meth_entry->second)) (insn);
  else
    invalid_dump_method (insn, 0);
}



// Dumper methods

void
InsnDumper::dump_cond_branch (Insn *insn)
{
  CondBranchInsn *cond_branch_insn = dynamic_cast<CondBranchInsn *> (insn);

  if (! cond_branch_insn)
    invalid_dump_method (insn, "CondBranchInsn");

  std::ostream &out = flow_graph_dumper.out;
  BB *target = cond_branch_insn->target ();
  out << "if (";
  Reg *cond = cond_branch_insn->condition ();
  if (cond)
    out << cond->name ();
  else
    out << "0";
  out << ") goto ";
  if (target)
    out << flow_graph_dumper.block_label (target);
  else
    out << "-";
}



// Dumper method dispatch machinery.


std::unordered_map<std::type_index, InsnDumper::DumpMeth> InsnDumper::dump_meths;


// Populate the DUMP_METHS data member.
//
void
InsnDumper::setup_dump_meths ()
{
  // If we've already done this setup, just return.
  //
  if (! dump_meths.empty ())
    return;

  dump_meths[typeid (CondBranchInsn)] = &InsnDumper::dump_cond_branch;
}


// Called when there's a mixup and dump method that expected an insn
// of type WANTED is called on the insn INSN which is not of that
// type.  Raises an exception.
//
void
InsnDumper::invalid_dump_method (Insn *insn, const char *wanted)
{
  std::string msg;
  if (wanted)
    {
      msg += "Invalid InsnDumper method called; wanted an insn of type ";
      msg += wanted;
      msg += " but got one of type ";
    }
  else
    {
      msg += "No InsnDumper method for insn of type ";
    }
  msg += typeid (*insn).name ();
  throw std::runtime_error (msg);
}
