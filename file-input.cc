// file-input.cc -- File input source
//
// Copyright © 2019  Miles Bader
//
// Author: Miles Bader <snogglethorpe@gmail.com>
// Created: 2019-12-13
//

#include "file-input.h"


// Make a new stream reading the file named FILE_NAME, which is in
// the source context SRC_CONTEXT.
//
// If LINE_ORIENTED is true (the default is false), this input will
// be in "line-oriented" mode, in which case character reading stops
// at the end of each line, until a new line is explicitly read.
//
FileInput::FileInput (const std::string &file_name, FileSrcContext &src_context,
		      bool line_oriented)
  : _inp_stream (file_name, std::ios_base::in),
    _line_oriented (line_oriented),
    _src_context (src_context), _src_file (_src_context.file (file_name))
{
  // Read the first line.
  //
  read_new_line ();
}


// Read a new line from this stream into the current line buffer,
// and return true if successful. If at the end of the file, return
// false.
//
bool
FileInput::read_new_line ()
{
  std::streamoff file_offs = _inp_stream.tellg ();

  _cur_line_offs = 0;

  if (std::getline (_inp_stream, _cur_line))
    {
      // Length of this line in the file, including the terminating
      // newline.
      //
      unsigned file_line_size = _cur_line.size () + 1;

      // When not in line-oriented mode, we explicitly represent
      // newlines.
      //
      if (! _line_oriented)
	_cur_line += '\n';

      _cur_line_max_offs = _cur_line.size ();

      _cur_line_src_loc = _src_file->add_line (file_offs, file_line_size);

      return true;
    }
  else
    {
      _cur_line_max_offs = 0;
      return false;
    }
}


// Deal appropriately with a read past at the end of the current
// line, returning true if the current line has been updated so
// there's more to read.
//
// In line-oriented mode, this just returns false.
//
// In non-line-oriented mode, this returns false only at the end of
// the file, and otherwise reads a newline and returns true.
//
bool
FileInput::read_past_eol ()
{
  if (_line_oriented)
    return false;
  else
    return read_new_line ();
}
