#ifndef __BB_IO_H__
#define __BB_IO_H__


class BB;
class FunDumper;


// A class for outputting text representations of a basic block.
//
class BBDumper
{
public:

  BBDumper (FunDumper &fun_dumper);

  // Write a text representation of BLOCK.
  //
  void dump (BB *block);


private:

  // Dumper for the function we're associated with.
  //
  FunDumper &fun_dumper;
};


#endif // __BB_IO_H__
