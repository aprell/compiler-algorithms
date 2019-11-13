// check-assertion.cc -- Assertion checking
//
// Copyright © 2019  Miles Bader
//
// Author: Miles Bader <snogglethorpe@gmail.com>
// Created: 2019-11-13
//

#include <stdexcept>

#include "check-assertion.h"


[[noreturn]] extern void check_assertion_failure (const std::string &msg)
{
  throw std::runtime_error (std::string ("Assertion failure: ") + msg);
}

[[noreturn]] void check_assertion_failure (const char *msg)
{
  check_assertion_failure (std::string (msg));
}
