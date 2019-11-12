// bitvec.h -- Fast bit vectors, for use as sets
//
// Copyright © 2019  Miles Bader
//
// Author: Miles Bader <snogglethorpe@gmail.com>
// Created: 2019-10-28
//

#ifndef __BITVEC_H__
#define __BITVEC_H__


#include <vector>
#include <cstdint>


// Fast bit vectors, for use as sets.
//
class Bitvec
{
public:

  Bitvec (unsigned size);

  bool get (unsigned index) const
  {
    return words[word_index (index)] & bit_mask (index);
  }
  void set (unsigned index, bool val)
  {
    Word widx = word_index (index);
    Word mask = bit_mask (index);
    if (val)
      words[widx] |= mask;
    else
      words[widx] &= ~mask;
  }
  void set (unsigned index) { set (index, true); }
  void clear (unsigned index) { set (index, false); }

  void set (const Bitvec &bv);
  void clear (const Bitvec &bv);

  void intersect (const Bitvec &bv);

  bool operator[] (unsigned index) const { return get (index); }
  Bitvec &operator|= (const Bitvec &other) { set (other); return *this; }

  // Return the number of bits set.
  //
  unsigned count () const;


private:

  typedef std::uint_fast32_t Word;

  static const unsigned WordsPerWord = sizeof (Word) * 8
;
  unsigned word_index (unsigned index) const
  {
    return index / WordsPerWord;
  }
  unsigned bit_pos (unsigned index) const
  {
    return index % WordsPerWord;
  }
  Word bit_mask (unsigned index) const
  {
    return 1 << bit_pos (index);
  }
  

  // Return the number of bits set in WORD.
  //
  static inline unsigned popcount (Word word);


  std::vector<Word> words;
};


#endif // __BITVEC_H__
