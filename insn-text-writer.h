// insn-text-writer.h -- Text-format output of an IR instruction
//
// Copyright © 2019  Miles Bader
//
// Author: Miles Bader <snogglethorpe@gmail.com>
// Created: 2019-11-02
//

#ifndef __INSN_TEXT_WRITER_H__
#define __INSN_TEXT_WRITER_H__

#include <unordered_map>
#include <typeinfo>
#include <typeindex>


class Insn;
class FunTextWriter;


// A class for outputting text representations of isns.
//
class InsnTextWriter
{
public:

  InsnTextWriter (FunTextWriter &fun_writer);

  // Write a text representation of INSN.
  //
  void write (Insn *insn);


private:

  // Return a string representation of the register REG
  //
  std::string reg_name (Reg *reg) const;

  // Return a string representation of the registers in the vector
  // REGS, starting from index START_IDX (default 0).
  //
  std::string reg_names (const std::vector<Reg *> &regs,
			 unsigned start_idx = 0)
    const;


  // Text writer for the function we're associated with.
  //
  FunTextWriter &fun_writer;


private:
  // Text writer methods for various insn types.

  void write_cond_branch (Insn *insn);
  void write_nop (Insn *insn);
  void write_calc (Insn *insn);
  void write_copy (Insn *insn);
  void write_fun_arg (Insn *insn);
  void write_fun_result (Insn *insn);
  void write_phi_fun (Insn *insn);
  void write_phi_fun_inp (Insn *insn);


private:
  // Text writer method dispatch machinery.  This is all global, and so
  // initialized only once.

  typedef void (InsnTextWriter::*WriteMeth) (Insn *insn);

  static std::unordered_map<std::type_index, WriteMeth> write_meths;

  // Populate the WRITE_METHS data member.
  //
  static void setup_write_meths ();

  // Called when there's a mixup and write method that expected an insn
  // of type WANTED is called on the insn INSN which is not of that
  // type.  Raises an exception.
  //
  void invalid_write_method (Insn *insn, const char *wanted);
};


#endif // __INSN_TEXT_WRITER_H__
