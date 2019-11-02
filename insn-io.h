#ifndef __INSN_IO_H__
#define __INSN_IO_H__

#include <unordered_map>
#include <typeinfo>
#include <typeindex>


class Insn;
class FunDumper;


// A class for outputting text representations of isns.
//
class InsnDumper
{
public:

  InsnDumper (FunDumper &fun_dumper);

  // Write a text representation of INSN.
  //
  void dump (Insn *insn);


private:

  // Dumper for the function we're associated with.
  //
  FunDumper &fun_dumper;


private:
  // Dumper methods for various insn types.

  void dump_cond_branch (Insn *insn);


private:
  // Dumper method dispatch machinery.  This is all global, and so
  // initialized only once.

  typedef void (InsnDumper::*DumpMeth) (Insn *insn);

  static std::unordered_map<std::type_index, DumpMeth> dump_meths;

  // Populate the DUMP_METHS data member.
  //
  static void setup_dump_meths ();

  // Called when there's a mixup and dump method that expected an insn
  // of type WANTED is called on the insn INSN which is not of that
  // type.  Raises an exception.
  //
  void invalid_dump_method (Insn *insn, const char *wanted);
};


#endif // __INSN_IO_H__
