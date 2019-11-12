// insn-text-reader.cc -- Text-format input of an IR instruction
//
// Copyright © 2019  Miles Bader
//
// Author: Miles Bader <snogglethorpe@gmail.com>
// Created: 2019-11-03
//

#include "insn-text-reader.h"

InsnTextReader::InsnTextReader (FunTextReader &_fun_reader)
  : fun_reader (_fun_reader)
{
  
}
