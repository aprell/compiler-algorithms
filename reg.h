// reg.h -- IR registers
//
// Copyright © 2019  Miles Bader
//
// Author: Miles Bader <snogglethorpe@gmail.com>
// Created: 2019-11-01
//

#ifndef __REG_H__
#define __REG_H__

#include <string>
#include <list>


class Fun;
class Insn;


// IR register, representing a value/storage-location.
//
class Reg
{
public:

  Reg (const std::string &name, Fun *fun = 0);
  ~Reg ();


  // Return the name of this register.
  //
  const std::string &name () const { return _name; }


  // Return a list of places this register is used.
  //
  const std::list<Insn *> uses () const { return _uses; }

  // Return a list of places this register is set.
  //
  const std::list<Insn *> defs () const { return _defs; }


  // Remember that this register is used in instruction INSN.
  // This does not effect INSN at all, it is up to the caller to do
  // so.
  //
  void add_use (Insn *insn) { _uses.push_back (insn); }

  // Remember that this register is set in instruction INSN.
  // This does not effect INSN at all, it is up to the caller to do
  // so.
  //
  void add_def (Insn *insn) { _defs.push_back (insn); }


  // Forget that this register is used in instruction INSN.
  // This does not effect INSN at all, it is up to the caller to do
  // so.
  //
  void remove_use (Insn *insn) { _uses.remove (insn); }

  // Forget that this register is set in instruction INSN.
  // This does not effect INSN at all, it is up to the caller to do
  // so.
  //
  void remove_def (Insn *insn) { _defs.remove (insn); }


  // Set the function this register is associated with to FUN.  This
  // will also remove the register from any previously associated
  // function, and add it to FUN.  FUN may be NULL.
  //
  void set_fun (Fun *fun);


private:

  // Name of this register.
  //
  std::string _name;

  // Function where this register is used.
  //
  Fun *_fun = 0;

  // Places this register is used.
  //
  std::list<Insn *> _uses;

  // Places this register is set.
  //
  std::list<Insn *> _defs;
};


#endif // __REG_H__


