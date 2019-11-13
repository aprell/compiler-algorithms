// check-assertion.h -- Assertion checking
//
// Copyright © 2019  Miles Bader
//
// Author: Miles Bader <snogglethorpe@gmail.com>
// Created: 2019-11-13
//

#ifndef __CHECK_ASSERTION_H__
#define __CHECK_ASSERTION_H__

#include <string>


[[noreturn]] extern void check_assertion_failure (const char *msg);
[[noreturn]] extern void check_assertion_failure (const std::string &msg);

template<typename T>
static inline void check_assertion (bool check, T msg)
{
  if (! check)
    check_assertion_failure (msg);
}


#endif // __CHECK_ASSERTION_H__

