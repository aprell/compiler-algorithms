// file-input.h -- File input source
//
// Copyright © 2019  Miles Bader
//
// Author: Miles Bader <snogglethorpe@gmail.com>
// Created: 2019-12-13
//

#ifndef __FILE_INPUT_H__
#define __FILE_INPUT_H__

#include <fstream>
#include <string>

#include "file-src-context.h"


// A file-oriented text input source, basically supports
// character-by-character input, along with source-location tracking.
//
class FileInput
{
public:

  // Make a new stream reading the file named FILE_NAME, which is in
  // the source context SRC_CONTEXT.
  //
  // If LINE_ORIENTED is true (the default is false), this input will
  // be in "line-oriented" mode, in which case character reading stops
  // at the end of each line, until a new line is explicitly read.
  //
  FileInput (const std::string &file_name, FileSrcContext &src_context,
	     bool line_oriented = false);


  //
  // Character-by-character input
  //

  // Return the next unread character in the current file, or zero if
  // there is no remaining unread character.
  //
  char peek () { return ensure_char () ? _cur_line[_cur_line_offs] : 0; }

  // Read and return the next unread character in the current file, or
  // zero if there is no remaining unread character.
  //
  char read_char () { return ensure_char () ? _cur_line[_cur_line_offs++] : 0; }

  // If possible, "unread" the previously ready character, making it
  // available for subsequent use.
  //
  // Unreading cannot move past line boundaries; an unread at the
  // beginning of a line will have no effect.
  //
  void unread_char () { if (_cur_line_offs > 0) --_cur_line_offs; }


  //
  // Multiple-character lookahead in current line
  //

  // Return the character at offset OFFS from the current position in
  // the current line.  If there are no characters remaining in the
  // line at that offset, return 0.
  //
  char peek_in_line (unsigned offs)
  {
    if (_cur_line_offs + offs >= _cur_line_max_offs)
      return 0;
    else
      return _cur_line[_cur_line_offs + offs];
  }

  // Read and discard COUNT characters in the current line, stopping
  // if the end of the line is reached.
  //
  void consume_in_line (unsigned count)
  {
    if (_cur_line_offs + count > _cur_line_max_offs)
      _cur_line_offs = _cur_line_max_offs;
    else
      _cur_line_offs += count;
  }


  //
  // End-of-line character reading.  These remove characters from the
  // end of the current line.
  //

  // Return the last unused character in the current line, or zero if
  // there is no remaining used character in the line.
  //
  char eol_peek () const
  {
    return at_eol () ? 0 : _cur_line[_cur_line_max_offs - 1];
  }

  // Read and return the last unused character in the current line, or
  // zero if there is no remaining unused character.
  //
  char eol_read_char_backwards ()
  {
    return at_eol () ? 0 : _cur_line[--_cur_line_max_offs];
  }


  //
  // Line handling
  //

  // Return true if this input is in line-oriented mode.
  //
  bool line_oriented () const { return _line_oriented; }

  // Make this input line-oriented or not, depending on LINE_ORIENTED.
  //
  void set_line_oriented (bool line_oriented)
  {
    _line_oriented = line_oriented;
  }

  // Return true if we're at the end of the current line.
  //
  bool at_eol () const { return _cur_line_offs >= _cur_line_max_offs; }

  // Return true if there are at least COUNT characters unread in the
  // current line.
  //
  bool avail_in_line (unsigned count) const
  {
    return _cur_line_offs + count <= _cur_line_max_offs;
  }


  // Read a new line from this stream into the current line buffer,
  // and return true if successful. If at the end of the file, return
  // false.
  //
  bool read_new_line ();

  // Return true when we've reached the end of the input file.
  //
  bool at_eof () const { return _at_eof; }


  //
  // Error handling
  //

  // Throw an exception containing the error message ERR at location
  // LOC in the file.
  //
  [[noreturn]] void error (const std::string &err, SrcContext::Loc loc) const
  {
    _src_context.error (loc, err);
  }

  // Throw an exception containing the error message ERR at the
  // current location in the file.
  //
  [[noreturn]] void error (const std::string &err) const
  {
    _src_context.error (cur_src_loc (), err);
  }


  // Return a SrcContext for this FileInput.
  //
  const SrcContext &src_context () const { return _src_context; }


  // Return a source-location for the current location in the input.
  //
  SrcContext::Loc cur_src_loc () const
  {
    return _cur_line_src_loc + _cur_line_offs;
  }


private:

  // Try to make sure there's at least one character to read at the
  // current location, and return true if successful.
  //
  // If at the end of the file, or doing line-oriented input and at
  // the end of the line, return false.
  //
  bool ensure_char () { return at_eol () ? read_past_eol () : true; }

  // Deal appropriately with a read past at the end of the current
  // line, returning true if the current line has been updated so
  // there's more to read.
  //
  // In line-oriented mode, this just returns false.
  //
  // In non-line-oriented mode, this returns false only at the end of
  // the file, and otherwise reads a newline and returns true.
  //
  bool read_past_eol ();


  // Current line we're parsing.
  //
  std::string _cur_line;

  // Current parsing offset into _CUR_LINE.
  //
  unsigned _cur_line_offs = 0;

  // Point past which we won't parse in _CUR_LINE.
  //
  unsigned _cur_line_max_offs = 0;

  // Source-location of the begining of the current line.
  //
  FileSrcContext::Loc _cur_line_src_loc = 0;


  // Stream we're reading from.
  //
  std::fstream _inp_stream;


  // True if in "line-oriented" mode, in which case character reading
  // stops at the end of each line, until a new line is explicitly
  // read.
  //
  bool _line_oriented = false;


  // True when we've reached the end of the input file.
  //
  bool _at_eof = false;


  // SrcContext we're using.
  //
  FileSrcContext &_src_context;

  // Specific file within _SRC_CONTEXT that we're reading.
  //
  FileSrcContext::File *_src_file;

};


#endif // __FILE_INPUT_H__
