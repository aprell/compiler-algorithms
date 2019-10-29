#include "bitvec.h"


void
Bitvec::set (const Bitvec &other)
{
  unsigned nwords = words.size ();
  for (unsigned i = 0; i < nwords; i++)
    words[i] |= other.words[i];
}

void
Bitvec::intersect (const Bitvec &other)
{
  unsigned nwords = words.size ();
  for (unsigned i = 0; i < nwords; i++)
    words[i] &= other.words[i];
}



bool test (const Bitvec &bv)
{
  return bv[8];
}

void test2 (Bitvec &bv)
{
  bv.set (8, true);
}

bool test3 (const Bitvec &bv, unsigned idx)
{
  return bv.get (idx);
}

void test4 (Bitvec &bv, unsigned idx)
{
  bv.set (idx, true);
}
