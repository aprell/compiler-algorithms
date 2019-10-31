#ifndef __INSN_H__
#define __INSN_H__


class BB;


class Insn
{
public:

  Insn (BB *block = 0);

  // Return the block this instruction is in.
  //
  BB *block () const { return _block; }

  // Asserts that this instruction is in block BLOCK.  Note that
  // nothing else is done, so it's up to the caller to maintain any
  // other invariants.
  //
  void set_block (BB *block) { _block = block; }

private:

  // The block this instruction is in.
  //
  BB *_block;
};



#endif // __INSN_H__
