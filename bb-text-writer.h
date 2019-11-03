#ifndef __BB_TEXT_WRITER_H__
#define __BB_TEXT_WRITER_H__


class BB;
class FunTextWriter;


// A class for outputting text representations of a basic block.
//
class BBTextWriter
{
public:

  BBTextWriter (FunTextWriter &fun_writer);


  // Write a text representation of BLOCK.  If NEXT_BLOCK is non-zero,
  // it is the next block that will be written; this is used to suppress
  // a goto statement between adjacent blocks.
  //
  void write (BB *block, BB *next_block = 0);


private:

  // Text writer for the function we're associated with.
  //
  FunTextWriter &fun_writer;
};


#endif // __BB_TEXT_WRITER_H__
