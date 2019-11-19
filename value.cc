// value.cc-- IR values
//
// Copyright © 2019  Miles Bader
//
// Author: Miles Bader <snogglethorpe@gmail.com>
// Created: 2019-11-18
//

#include "fun.h"
#include "insn.h"

#include "value.h"


// Return a new value with integer value INT_VALUE.
// If FUN is non-NULL, the new value is added to FUN.
//
Value::Value (int int_value, Fun *fun)
  : _int_value (int_value)
{
  set_fun (fun);
}

Value::~Value ()
{
  set_fun (0);
}


// Set the function this value is associated with to FUN.  This
// will also remove the value from any previously associated
// function, and add it to FUN.  FUN may be NULL.
//
void
Value::set_fun (Fun *fun)
{
  if (fun != _fun)
    {
      if (_fun)
	_fun->remove_value (this);
      if (fun)
	fun->add_value (this);
      _fun = fun;
    }
}
