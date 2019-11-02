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

  // Write a text representation of BLOCK.
  //
  void write (BB *block);


private:

  // Text writer for the function we're associated with.
  //
  FunTextWriter &fun_writer;
};


#endif // __BB_TEXT_WRITER_H__
