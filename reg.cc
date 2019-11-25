// reg.cc-- IR registers
//
// Copyright © 2019  Miles Bader
//
// Author: Miles Bader <snogglethorpe@gmail.com>
// Created: 2019-11-17
//

#include "fun.h"
#include "insn.h"
#include "value.h"

#include "reg.h"


// Return a new register called NAME.  If FUN is non-NULL, the
// register is added to FUN.
//
Reg::Reg (const std::string &name, Fun *fun)
  : _name (name)
{
  set_fun (fun);
}

// Return a new unnamed register with constant value VALUE.
// If VALUE is in a function, the register is added to that function
// as well.
//
Reg::Reg (Value *value)
  : _value (value)
{
  Fun *fun = value->fun ();
  if (fun)
    set_fun (fun);
}

Reg::~Reg ()
{
  // If this is an SSA prototype, remove it from its value registers.
  //
  for (auto val_reg : _ssa_values)
    val_reg->_ssa_proto = 0;
  _ssa_values.clear ();

  // If this is an SSA value, remove it from its prototype register.
  //
  if (_ssa_proto)
    {
      _ssa_proto->_ssa_values.remove (this);
      _ssa_proto = 0;
    }

  // Remove this register from every place it's used / set.

  while (! _uses.empty ())
    _uses.front ()->change_arg (this, 0);

  while (! _defs.empty ())
    _defs.front ()->change_result (this, 0);

  set_fun (0);
}


// Set the function this register is associated with to FUN.  This
// will also remove the register from any previously associated
// function, and add it to FUN.  FUN may be NULL.
//
void
Reg::set_fun (Fun *fun)
{
  if (fun != _fun)
    {
      if (_fun)
	_fun->remove_reg (this);
      if (fun)
	fun->add_reg (this);
      _fun = fun;
    }
}


// Return a new individual value register for SSA-form, which will
// have this register as its prototype.
//
Reg *
Reg::make_ssa_value ()
{
  std::string val_name = _name;
  val_name += '.';
  val_name += std::to_string (_ssa_values.size ());

  Reg *val_reg = new Reg (val_name, _fun);

  val_reg->_ssa_proto = this;
  _ssa_values.push_back (val_reg);

  return val_reg;
}
