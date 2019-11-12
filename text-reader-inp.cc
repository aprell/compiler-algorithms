// text-reader-inp.cc -- Input source for IR text-format input classes
//
// Copyright © 2019  Miles Bader
//
// Author: Miles Bader <snogglethorpe@gmail.com>
// Created: 2019-11-03
//

#include "text-reader-inp.h"


// Read and return an unsigned integer, or signal an error if none
// can be read.
//
unsigned
TextReaderInp::read_unsigned ()
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
      parse_error ("Expected unsigned integer");
    }
}

// Read and return an identifier name (/[_a-zA-Z][_a-zA-Z0-9]*/), or
// signal an error if none.
//
std::string
TextReaderInp::read_id ()
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
      parse_error ("Expected identifier");
    }
}


// If the character CH is the next unread character, skip it,
// otherwise signal an error.
//
void
TextReaderInp::expect (char ch)
{
  if (! skip (ch))
    parse_error (std::string ("Expected '") + ch + "'");
}

// If the keyword KEYW follows the current position, skip it, 
// otherwise signal an error.
//
void
TextReaderInp::expect (const char *keyw)
{
  if (! skip (keyw))
    parse_error (std::string ("Expected \"") + keyw + "\"");
}


// If the character CH is the next unread character, skip it, and
// return true, otherwise return false.
//
bool
TextReaderInp::skip (char ch)
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
TextReaderInp::skip (const char *keyw)
{
  skip_whitespace ();

  unsigned keyw_len = strlen (keyw);

  if (_cur_line_offs + keyw_len > _cur_line_max_offs)
    return false;

  if (is_id_cont_char (_cur_line[_cur_line_offs + keyw_len]))
    return false;

  for (unsigned i = 0; i < keyw_len; i++)
    if (_cur_line[_cur_line_offs + i] != keyw[i])
      return false;

  _cur_line_offs += keyw_len;

  return true;
}


// If the character CH is the next unread character at the end of
// the current line, remove it, and return true, otherwise return
// false.
//
bool
TextReaderInp::skip_eol (char ch)
{
  skip_eol_whitespace ();

  if (peek_eol () == ch)
    {
      read_eol_char ();
      return true;
    }
  else
    {
      return false;
    }
}


// Read a new line, and return true if successful.
//
bool
TextReaderInp::read_new_line ()
{
  _cur_line_offs = 0;
  if (std::getline (_inp_stream, _cur_line))
    {
      _cur_line_max_offs = _cur_line.length ();
      return true;
    }
  else
    {
      _cur_line_max_offs = 0;
      return false;
    }
}


// Throw an exception containing the error message ERR.
//
void
TextReaderInp::parse_error (const std::string &err) const
{
  throw std::runtime_error (cur_line_parse_desc () + "Parse error: " + err);
}


// Return a string showing the current line and parse position.
//
std::string
TextReaderInp::cur_line_parse_desc () const
{
  std::string desc (_cur_line);
  desc += '\n';
  for (unsigned i = 0; i < _cur_line_offs; i++)
    desc += ' ';
  desc += '^';
  desc += '\n';
  return desc;
}
