// fun.cc -- IR functions
//
// Copyright © 2019  Miles Bader
//
// Author: Miles Bader <snogglethorpe@gmail.com>
// Created: 2019-11-02
//

#include "reg.h"

#include "fun.h"


Fun::Fun ()
{
  // We initialize _ENTRY_BLOCK / _EXIT_BLOCK here to ensure the rest
  // of our state is already initialized, as the BB constructor will
  // add the block to this function..

  _entry_block = new BB (this);
  _exit_block = new BB (this);
}


Fun::~Fun ()
{
  while (! _blocks.empty ())
    delete _blocks.front ();

  while (! _regs.empty ())
    delete _regs.front ();
}


// Remove BLOCK from this function.
//
void
Fun::remove_block (BB *block)
{
  _blocks.remove (block);

  if (_entry_block == block)
    _entry_block = 0;
  if (_exit_block == block)
    _exit_block = 0;
}
