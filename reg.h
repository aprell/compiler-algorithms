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
class Value;


// IR register, representing a value/storage-location.
//
class Reg
{
public:

  // Return a new register called NAME.  If FUN is non-NULL, the
  // register is added to FUN.
  //
  Reg (const std::string &name, Fun *fun = 0);

  // Return a new unnamed register with constant value VALUE.
  // If VALUE is in a function, the register is added to that function
  // as well.
  //
  Reg (Value *value);

  ~Reg ();


  // Return the name of this register.
  //
  const std::string &name () const { return _name; }


  // Return this register's known value, of NULL if it has none.
  //
  Value *value () const { return _value; }

  // Return true if this is a constant-valued unnamed register.
  //
  bool is_constant () const { return _name.size () == 0 && _value; }


  // Return a reference to a read-only list of places this register is
  // used.
  //
  const std::list<Insn *> &uses () const { return _uses; }

  // Return a reference to a read-only list of places this register is
  // set.
  //
  const std::list<Insn *> &defs () const { return _defs; }


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


  // If this register is a particular value in SSA-form, return the
  // original register it corresponds to, otherwise known as its
  // "prototype," otherwise return NULL.
  //
  Reg *ssa_proto () const { return _ssa_proto; }

  // In SSA-form, return a list of individal value registers
  // corresponding to this prototype register, otherwiser return an
  // empty list.
  //
  const std::list<Reg *> ssa_values () const { return _ssa_values; }

  // Return a new individual value register for SSA-form, which will
  // have this register as its prototype.
  //
  Reg *make_ssa_value ();


private:

  // Name of this register.
  //
  std::string _name;

  // Function where this register is used.
  //
  Fun *_fun = 0;

  // If non-NULL, this register's known value.
  //
  Value *_value = 0;

  // Places this register is used.
  //
  std::list<Insn *> _uses;

  // Places this register is set.
  //
  std::list<Insn *> _defs;

  // If this register is a particular value in SSA-form, the original
  // register it corresponds to, otherwise known as its "prototype,"
  // otherwise NULL.
  //
  Reg *_ssa_proto;

  // In SSA-form, the list of individal values corresponding to this
  // prototype.
  //
  std::list<Reg *> _ssa_values;
};


#endif // __REG_H__


