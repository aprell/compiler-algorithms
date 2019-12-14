// src-context.h -- Source context
//
// Copyright © 2019  Miles Bader
//
// Author: Miles Bader <snogglethorpe@gmail.com>
// Created: 2019-12-08
//

#ifndef __SRC_CONTEXT_H__
#define __SRC_CONTEXT_H__

#include <string>


// A source-context.  This is an abstract class.
//
class SrcContext
{
public:

  // A source location within this context.
  //
  typedef unsigned long Loc;

  // Signal an exception for an error at location LOC within this
  // context with message MSG.
  //
  [[noreturn]] virtual void error (Loc loc, const std::string &msg) const = 0;
};


#endif // __SRC_CONTEXT_H__
