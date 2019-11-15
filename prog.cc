// prog.cc -- IR program
//
// Copyright © 2019  Miles Bader
//
// Author: Miles Bader <snogglethorpe@gmail.com>
// Created: 2019-11-14
//

#include <stdexcept>

#include "fun.h"

#include "prog.h"


Prog::~Prog ()
{
  for (auto [_, fun] : _funs)
    delete fun;
}


// Add FUN to this program, with the name NAME.  Subsequent to this
// call, FUN is owned by this program, which is responsible for
// deallocating it.
//
void
Prog::add_fun (const std::string &name, Fun *fun)
{
  Fun *&stored_fun =_funs[name];
  if (stored_fun)
    throw std::runtime_error (std::string ("Duplicate function definition \"") + name + "\"");
  stored_fun = fun;
}
