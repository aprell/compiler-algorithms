// reg.cc-- IR registers
//
// Copyright © 2019  Miles Bader
//
// Author: Miles Bader <snogglethorpe@gmail.com>
// Created: 2019-11-17
//

#include "fun.h"
#include "insn.h"

#include "reg.h"


Reg::Reg (const std::string &name, Fun *fun)
  : _name (name)
{
  set_fun (fun);
}

Reg::~Reg ()
{
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

