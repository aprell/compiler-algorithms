#ifndef __BB_TEXT_READER_H__
#define __BB_TEXT_READER_H__


class BB;
class FunTextReader;


// A class for outputting text representations of a basic block.
//
class BBTextReader
{
public:

  BBTextReader (FunTextReader &fun_reader);


  // Write a text representation of BLOCK.
  //
  void write (BB *block);


private:

  // Text reader for the function we're associated with.
  //
  FunTextReader &fun_reader;
};


#endif // __BB_TEXT_READER_H__
