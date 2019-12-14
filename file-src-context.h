// file-src-context.h -- File-oriented source context
//
// Copyright © 2019  Miles Bader
//
// Author: Miles Bader <snogglethorpe@gmail.com>
// Created: 2019-12-13
//

#ifndef __FILE_SRC_CONTEXT_H__
#define __FILE_SRC_CONTEXT_H__

#include <fstream>
#include <vector>
#include <map>

#include "src-context.h"


// A file-oriented source-context, which tracks source locations
// inside files.
//
class FileSrcContext : public SrcContext
{
public:

  typedef std::streampos FilePos;
  typedef std::streamoff FileOffs;

  class File
  {
  public:

    struct Chunk;


    File (const std::string &name, FileSrcContext &src_context)
      : _name (name), _src_context (src_context)
    { }


    // Return the beginning source-location for a line in this file
    // with a byte range [BOL_OFFS, BOL_OFFS+LEN).
    //
    // A source-location for a position within the line can be
    // calculated just by adding the offset within the line to the
    // returned beginning-of-line source-location.
    //
    // This interface is intended for reading through files
    // sequentially, possibly multiple times.  As a result, BOL_OFFS
    // and LEN must meet the following constraints:
    //
    //  + If the byte range [BOL_OFFS, BOL_OFFS+LEN) in this file has
    //    already been added, nothing is changed, and existing
    //    information is used.
    //
    //  + Otherwise, BOL_OFFS must either be zero, or must
    //    _immediately_ follow the file location of the last line in
    //    this file which was previously added.
    //
    Loc add_line (FilePos bol_offs, FileOffs len);

    // Return the name of this file.
    //
    const std::string &name () const { return _name; }


  private:

    // Name of this file.
    //
    std::string _name;

    // Chunks inside the file.
    //
    std::vector<Chunk *> _chunks;

    // Source context this file is in.
    //
    FileSrcContext &_src_context;
  };


  FileSrcContext () { }


  // Signal an exception for an error at location LOC within this
  // context with message MSG.
  //
  [[noreturn]] virtual void error (Loc loc, const std::string &msg) const;


  // Return file-specific info for the file FILE_NAME in this source context.
  //
  File *file (const std::string &file_name);


  // Return info about the source-location LOC in the following
  // out-parameters:
  //  + FILE_NAME is the name of the source file LOC is from
  //  + LINE_NUM is the line number of the line LOC is from in FILE_NAME
  //  + LINE_OFFS is the offset of LOC in that line
  //  + LINE_text is the actual text of the line
  //
  void loc_source (Loc loc,
		   std::string &file_name,
		   unsigned &line_num, unsigned &line_offs,
		   std::string &line)
    const;

  
private:

  // Return the chunk LOC is inside.
  //
  File::Chunk *loc_chunk (Loc loc) const;


  // The number of bits inside a FileSrcContext::Loc descriptor which
  // correspond to the chunk-offset of the location.
  //
  static const unsigned LOC_OFFS_BITS = 16;

  // The maximum size of a chunk.  This is also the size of the region
  // within the Loc address space that each chunk represents.
  //
  static const unsigned MAX_CHUNK_SIZE = (1 << LOC_OFFS_BITS);

  // Files tracked by this source context.
  //
  std::map<const std::string, File *> _files;

  // A series of "chunks", each representing MAX_CHUNK_SIZE bytes
  // within the Loc address space, and each referring to some part of
  // some file.
  //
  std::vector<File::Chunk *> _chunks;
};


#endif // __FILE_SRC_CONTEXT_H__
