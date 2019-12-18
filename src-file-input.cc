// src-file-input.cc -- Source-code file input
//
// Copyright © 2019  Miles Bader
//
// Author: Miles Bader <snogglethorpe@gmail.com>
// Created: 2019-11-03
//

#include "src-file-input.h"


// Read and return an unsigned integer, or signal an error if none
// can be read.
//
unsigned
SrcFileInput::read_unsigned ()
{
  skip_whitespace ();

  char ch = peek ();
  if (ch >= '0' && ch <= '9')
    {
      unsigned num = 0;
      do
	num = num * 10 + (read_char () - '0');
      while ((ch = peek ()) >= '0' && ch <= '9');
      return num;
    }
  else
    {
      error ("Expected unsigned integer");
    }
}

// Read and return a signed integer, or signal an error if none
// can be read.
//
int
SrcFileInput::read_int ()
{
  skip_whitespace ();

  bool is_negative = skip ('-');

  char ch = peek ();
  if (ch >= '0' && ch <= '9')
    {
      int num = 0;

      do
	num = num * 10 + (read_char () - '0');
      while ((ch = peek ()) >= '0' && ch <= '9');

      return is_negative ? -num : num;
    }
  else
    {
      error ("Expected integer");
    }
}


// Read and return an identifier name (/[_a-zA-Z][_a-zA-Z0-9]*/), or
// signal an error if none.
//
std::string
SrcFileInput::read_id ()
{
  skip_whitespace ();

  char ch = peek ();
  if (is_id_start_char (ch))
    {
      std::string id;
      do
	id += read_char ();
      while (is_id_cont_char (peek ()));
      return id;
    }
  else
    {
      error ("Expected identifier");
    }
}


// Read and return a string terminated by TERMINATOR.
// TERMINATOR is consumed, but not put into the returned string.
//
std::string
SrcFileInput::read_delimited_string (char terminator)
{
  std::string value;

  while (! skip (terminator))
    {
      if (at_eof ())
	error (std::string ("Missing terminator '") + terminator + "'");

      value += read_char ();
    }

  return value;
}


// If the character CH is the next unread character, skip it,
// otherwise signal an error.
//
void
SrcFileInput::expect (char ch)
{
  if (! skip (ch))
    error (std::string ("Expected '") + ch + "'");
}

// If the keyword KEYW follows the current position, skip it, 
// otherwise signal an error.
//
void
SrcFileInput::expect (const char *keyw)
{
  if (! skip (keyw))
    error (std::string ("Expected \"") + keyw + "\"");
}


// If the character CH is the next unread character, skip it, and
// return true, otherwise return false.
//
bool
SrcFileInput::skip (char ch)
{
  skip_whitespace ();

  if (peek () == ch)
    {
      read_char ();
      return true;
    }
  else
    {
      return false;
    }
}

// If the keyword KEYW follows the current position, skip it, and
// return true, otherwise return false.
//
bool
SrcFileInput::skip (const char *keyw)
{
  skip_whitespace ();

  unsigned keyw_len = strlen (keyw);

  if (! avail_in_line (keyw_len))
    return false;

  if (is_id_cont_char (peek_in_line (keyw_len)))
    return false;

  for (unsigned i = 0; i < keyw_len; i++)
    if (peek_in_line (i) != keyw[i])
      return false;

  consume_in_line (keyw_len);

  return true;
}


// If the character CH is the last unused character at the end of
// the current line, remove it, and return true, otherwise return
// false.
//
bool
SrcFileInput::eol_skip_backwards (char ch)
{
  eol_skip_whitespace_backwards ();

  if (eol_peek () == ch)
    {
      eol_read_char_backwards ();
      return true;
    }
  else
    {
      return false;
    }
}
