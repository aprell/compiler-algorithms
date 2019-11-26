// check-assertion.cc -- Assertion checking
//
// Copyright © 2019  Miles Bader
//
// Author: Miles Bader <snogglethorpe@gmail.com>
// Created: 2019-11-13
//

#include <stdexcept>

#include "check-assertion.h"


[[noreturn]] extern void check_assertion_failure_with_pos (
			   const std::string &msg,
			   const char *file,
			   unsigned line)
{
  std::string loc ("Assertion failure: ");
  loc += file;
  loc += ":";
  loc += std::to_string (line);
  loc += ": ";
  throw std::runtime_error (loc + msg);
}

[[noreturn]] void check_assertion_failure_with_pos (
		    const char *msg,
		    const char *file,
		    unsigned line)
{
  check_assertion_failure_with_pos (std::string (msg), file, line);
}
