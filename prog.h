// prog.h -- IR program
//
// Copyright © 2019  Miles Bader
//
// Author: Miles Bader <snogglethorpe@gmail.com>
// Created: 2019-11-14
//

#ifndef __PROG_H__
#define __PROG_H__

#include <unordered_map>

#include "fun.h"


// IR program, containing a set of functions and other global program state.
//
class Prog
{
public:

  ~Prog ();


  // Add FUN to this program, with the name NAME.  Subsequent to this
  // call, FUN is owned by this program, which is responsible for
  // deallocating it.
  //
  void add_fun (const std::string &name, Fun *fun);

  // Return a reference to a read-only map containing the mapping of
  // names to functions in this program.
  //
  const std::unordered_map<std::string, Fun *> &functions () const { return _funs; }


private:

  // A read-only map containing the mapping of names to functions in
  // this program.
  //
  std::unordered_map<std::string, Fun *> _funs;

};


#endif // __PROG_H__
