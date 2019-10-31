#include "bb.h"

#include "insn.h"


Insn::Insn (BB *block)
  : _block (block)
{
  if (block)
    block->add_insn (this);
}
