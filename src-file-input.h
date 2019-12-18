// src-file-input.h -- Source-code file input
//
// Copyright © 2019  Miles Bader
//
// Author: Miles Bader <snogglethorpe@gmail.com>
// Created: 2019-12-13
//

#ifndef __SRC_FILE_INPUT_H__
#define __SRC_FILE_INPUT_H__

#include <string>

#include "file-input.h"


// A text input source, adds somewhat higher-level methods on top of a
// file stream for source-code input.
//
class SrcFileInput : public FileInput
{
public:

  // Make a new source-file stream reading the file named FILE_NAME,
  // which is in the source context SRC_CONTEXT.
  //
  // If LINE_ORIENTED is true (the default is false), this input will
  // be in "line-oriented" mode, in which case character reading stops
  // at the end of each line, until a new line is explicitly read.
  //
  SrcFileInput (const std::string &file_name, FileSrcContext &src_context,
		bool line_oriented = false)
    : FileInput (file_name, src_context, line_oriented)
  { }


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


  // Read and return a string terminated by TERMINATOR.
  // TERMINATOR is consumed, but not put into the returned string.
  //
  std::string read_delimited_string (char terminator);


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
    while (is_whitespace (peek ()))
      read_char ();
  }


  // If the character CH is the last unused character at the end of
  // the current line, remove it, and return true, otherwise return
  // false.
  //
  bool eol_skip_backwards (char ch);

  // Remove any whitespace characters at the end of the current line.
  //
  void eol_skip_whitespace_backwards ()
  {
    while (! at_eol () && is_whitespace (eol_peek ()))
      eol_read_char_backwards ();
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
    return ch == ' ' || ch == '\t' || ch == '\n';
  }

};


#endif // __SRC_FILE_INPUT_H__
