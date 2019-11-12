// bitvec.cc -- Fast bit vectors, for use as sets
//
// Copyright © 2019  Miles Bader
//
// Author: Miles Bader <snogglethorpe@gmail.com>
// Created: 2019-10-28
//

#if __cpp_lib_bitops
# include <bit>
#endif


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


inline unsigned
Bitvec::popcount (Word word)
{
#if __cpp_lib_bitops
  return std::popcount (word);
#else
  unsigned count = 0;
  while (word)
    {
      count++;
      word &= (word - 1);
    }
  return count;
#endif
}  


// Return the number of bits set.
//
unsigned
Bitvec::count () const
{
  unsigned count = 0;
  for (Word word : words)
    count += popcount (word);
  return count;
}

