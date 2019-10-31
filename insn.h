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

  // Do instruction-specific setup after this instruction has been
  // added to block BLOCK.  At the point this is called, this
  // instruction is in BLOCK's instruction list, but nothing else has
  // been done.
  //
  virtual void set_block (BB *block) { _block = block; }


private:

  // The block this instruction is in.
  //
  BB *_block;
};



#endif // __INSN_H__
