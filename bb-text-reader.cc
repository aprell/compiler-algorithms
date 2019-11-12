// bb-text-reader.cc -- Text-format input of an IR basic block
//
// Copyright © 2019  Miles Bader
//
// Author: Miles Bader <snogglethorpe@gmail.com>
// Created: 2019-11-03
//

#include "bb-text-reader.h"


BBTextReader::BBTextReader (FunTextReader &_fun_reader)
  : fun_reader (_fun_reader)
{
}
