// text-reader-inp.h -- Input source for IR text-format input classes
//
// Copyright © 2019  Miles Bader
//
// Author: Miles Bader <snogglethorpe@gmail.com>
// Created: 2019-11-03
//

#ifndef __TEXT_READER_INP_H__
#define __TEXT_READER_INP_H__

#include <istream>
#include <string>


// A text input source for *TextReader classes, adds somewhat
// higher-level methods on top of a stream to do line-by-line input.
//
class TextReaderInp
{
public:

  TextReaderInp (std::istream &inp_stream)
    : _inp_stream (inp_stream)
  {
  }


  // Read and return an unsigned integer, or signal an error if none
  // can be read.
  //
  unsigned read_unsigned ();

  // Read and return a signed integer, or signal an error if none
  // can be read.
  //
  int read_int ();

  // Read and return an identifier name (/[_a-zA-Z][_a-zA-Z0-9]*/), or
  // signal an error if none.
  //
  std::string read_id ();


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

  // Skip any whitespace characters at the current position.
  //
  void skip_whitespace ()
  {
    while (! at_eol () && is_whitespace (peek ()))
      read_char ();
  }


  // If the character CH is the next unread character at the end of
  // the current line, remove it, and return true, otherwise return
  // false.
  //
  bool skip_eol (char ch);

  // Skip any whitespace characters at the end of the current line.
  //
  void skip_eol_whitespace ()
  {
    while (! at_eol () && is_whitespace (peek_eol ()))
      read_eol_char ();
  }


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


  // Return the next unread character in the current line, or zero if
  // there is no remaining unread character.
  //
  char peek () const { return at_eol () ? 0 : _cur_line[_cur_line_offs]; }

  // Return the last unused character in the current line, or zero if
  // there is no remaining unread character.
  //
  char peek_eol () const { return at_eol () ? 0 : _cur_line[_cur_line_max_offs - 1]; }

  // Read and return the next unread character in the current line, or
  // zero if there is no remaining unread character.
  //
  char read_char () { return at_eol () ? 0 : _cur_line[_cur_line_offs++]; }

  // Read and return the last unread character in the current line, or
  // zero if there is no remaining unread character.
  //
  char read_eol_char () { return at_eol () ? 0 : _cur_line[--_cur_line_max_offs]; }

  // Return true if we're at the end of the current line.
  //
  bool at_eol () const { return _cur_line_offs >= _cur_line_max_offs; }

  
  // Read a new line, and return true if successful.
  //
  bool read_new_line ();


  // Throw an exception containing the error message ERR.
  //
  [[noreturn]] void parse_error (const std::string &err) const;


  // Return a string showing the current line and parse position.
  //
  std::string cur_line_parse_desc () const;


private:

  // Current line we're parsing.
  //
  std::string _cur_line;

  // Current parsing offset into _CUR_LINE.
  //
  unsigned _cur_line_offs = 0;

  // Point past which we won't parse in _CUR_LINE.
  //
  unsigned _cur_line_max_offs = 0;

  // Stream we're reading from.
  //
  std::istream &_inp_stream;

};


#endif // __TEXT_READER_INP_H__
