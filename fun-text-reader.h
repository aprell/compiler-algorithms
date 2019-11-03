#ifndef __FUN_TEXT_READER_H__
#define __FUN_TEXT_READER_H__

#include <istream>
#include <string>
#include <sstream>
#include <unordered_map>

#include "insn-text-reader.h"
#include "bb-text-reader.h"


class Fun;


// A class for reading a text representations of a function.
//
class FunTextReader
{
public:

  FunTextReader (std::istream &in);


  // Read a text representation of a function, and return the new function..
  //
  Fun *read ();


  // Read a new line, and return true if successful.
  //
  bool read_line ();

  // Return the next unread character in the current line.
  //
  char peek () { return is_eol () ? 0 : cur_line[cur_line_offs]; }

  // If the character CH is the next unread character, do nothing,
  // otherwise signal an error.
  //
  void expect (char ch);

  // If the keyword KEYW follows the current position, skip it, 
  // otherwise signal an error.
  //
  void expect (const char *keyw);

  // If the character CH is the next unread character, skip it, and
  // return true, otherwise return false;,
  //
  bool skip (char ch);

  // If the keyword KEYW follows the current position, skip it, and
  // return true, otherwise return false.
  //
  bool skip (const char *keyw);

  // Read and return an unsigned integer, or signal an error if none
  // can be read.
  //
  unsigned read_unsigned ();

  // Read and return an identifier name (/[_a-zA-Z][_a-zA-Z0-9]*/), or
  // signal an error if none.
  //
  std::string read_id ();

  // Read a block label.
  //
  BB *read_label () { return label_block (read_id ()); }

  // Read a register (which must exist).
  //
  Reg *read_reg ();

  // Return true if CH can start an identifier.
  //
  bool is_id_start_char (char ch)
  {
    return
      ch == '_'
      || (ch >= 'a' && ch <= 'z')
      || (ch >= 'A' && ch <= 'Z');
  }

  // Return true if CH can continue an identifier.
  //
  bool is_id_cont_char (char ch)
  {
    return
      ch == '_'
      || (ch >= 'a' && ch <= 'z')
      || (ch >= 'A' && ch <= 'Z')
      || (ch >= '0' && ch <= '9');
  }

  // Return true if CH is a whitespace character.
  //
  bool is_whitespace (char ch)
  {
    return ch == ' ' || ch == '\t';
  }

  // Skip any whitespace characters.
  //
  void skip_whitespace ()
  {
    while (! is_eol () && is_whitespace (peek ()))
      read_char ();
  }


  // Read and return a character.
  //
  char read_char () { return is_eol () ? 0 : cur_line[cur_line_offs++]; }


  // Return true if we're at the end of the current line.
  //
  bool is_eol () const { return cur_line_offs == cur_line.length (); }


  // Throw an exception containing the error message ERR.
  //
  [[noreturn]] void parse_error (const std::string &err) const;


  // Return a string showing the current line and parse position.
  //
  std::string cur_line_parse_desc () const;


  // Return the block corresponding to the label LABEL.  If no such
  // label has yet been encountered, a new block is added and returned.
  //
  BB *label_block (const std::string &label);


  //
  // The following data members are all public, and reflect common
  // state during writing.
  //


  // Stream we're writing to.
  //
  std::istream &in;

  // Text reader for insns.
  //
  InsnTextReader insn_reader;

  // Text reader for blocks.
  //
  BBTextReader block_reader;


private:

  // Parse the contents of a function.
  //
  void parse_fun ();

  // Clear any parsing state used while parsing a function.
  //
  void clear_state ();


  // Function we're currently reading, or zero if none.
  //
  Fun *cur_fun = 0;

  // Block we're currently reading, or zero if none.
  //
  BB *cur_block = 0;


  // Current line we're parsing.
  //
  std::string cur_line;

  // Offset into CUR_LINE.
  //
  unsigned cur_line_offs;

  std::unordered_map<std::string, BB *> labeled_blocks;

  std::unordered_map<std::string, Reg *> registers;

};


#endif // __FUN_TEXT_READER_H__
