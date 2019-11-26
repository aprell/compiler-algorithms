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


[[noreturn]] extern void check_assertion_failure_with_pos (const char *msg,
							   const char *file,
							   unsigned line);
[[noreturn]] extern void check_assertion_failure_with_pos (const std::string &msg,
							   const char *file,
							   unsigned line);

template<typename T>
static inline void check_assertion_with_pos (bool check, T msg,
					     const char *file,
					     unsigned line)
{
  if (! check)
    check_assertion_failure_with_pos (msg, file, line);
}


#define check_assertion(check, msg) \
  check_assertion_with_pos (check, msg, __FILE__, __LINE__)

#define check_assertion_failure(msg) \
  check_assertion_failure_with_pos (msg, __FILE__, __LINE__)


#endif // __CHECK_ASSERTION_H__

