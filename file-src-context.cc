// file-src-context.cc -- File-oriented source context
//
// Copyright © 2019  Miles Bader
//
// Author: Miles Bader <snogglethorpe@gmail.com>
// Created: 2019-12-13
//

#include <stdexcept>
#include <map>

#include "check-assertion.h"

#include "file-src-context.h"


// Return file-specific info for the file FILE_NAME in this source context.
//
FileSrcContext::File *
FileSrcContext::file (const std::string &file_name)
{
  File *&file_entry = _files[file_name];

  if (! file_entry)
    file_entry = new File (file_name, *this);

  return file_entry;
}



// A "chunk" within the FileSrcContext::Loc address space.  Each chunk
// corresponds to MAX_CHUNK_SIZE bytes within the Loc address space,
// and refers to a range of locations within some file up to that size
// (the size of the file range represented by a chunk may be smaller
// than MAX_CHUNK_SIZE, in which case the remaining address space is
// just unused).
//
struct FileSrcContext::File::Chunk
{
  Chunk (File *_file, FilePos _file_pos, FileOffs _size,
	 unsigned _line_offs = 0)
    : file (_file), loc (0 /* set below */),
      file_pos (_file_pos), size (_size),
      line_offs (_line_offs),
      line_num (0 /* set below */), num_lines (1)
  {
    // Global state for this chunk.
    //
    FileSrcContext &src_context = file->_src_context;
    std::vector<Chunk *> &global_chunks = src_context._chunks;

    // File state.
    //
    std::vector<Chunk *> &file_chunks = file->_chunks;

    // Check chunk ordering constraints and set line number.
    //
    if (file_chunks.empty ())
      {
	// first chunk in this file

	check_assertion (_file_pos == 0,
			 "FileSrcContext line not added contiguously");

	line_num = 1;
      }
    else
      {
	// This line follows some other line

	Chunk *prev_chunk = file_chunks.back ();

	check_assertion (prev_chunk->file_pos + prev_chunk->size == _file_pos,
			 "FileSrcContext line not added contiguously");

	line_num = prev_chunk->line_num + prev_chunk->num_lines;
      }

    // The source-location of this chunks corresponds to its global
    // position.
    //
    unsigned chunk_index = global_chunks.size ();
    loc = chunk_index << LOC_OFFS_BITS;

    // Add this chunk to both the file and global chunk vectors.
    //
    file->_chunks.push_back (this);
    global_chunks.push_back (this);
  }


  // File this chunk is inside.
  //
  File *file;

  // Source-location of the beginning of this chunk.  Locations within
  // the chunk are just LOC plus the offset within the chunk.
  //
  Loc loc;

  // Position of the beginning of this chunk inside the file.
  //
  FilePos file_pos;

  // Size of this chunk.  Chunks cannot be larger than
  // FileSrcContext::MAX_CHUNK_SIZE.
  //
  FileOffs size;

  // Offset of the beginning of this chunk inside the line it's in.
  // For most chunks this is 0, but very long lines may span
  // multiple chunks, in which case chunks after the first will have
  // non-zero line offsets.
  //
  unsigned line_offs;

  // Line-number of the first line within this chunk.
  //
  unsigned line_num;

  // Number of lines within this chunk.
  //
  unsigned num_lines;
};


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
FileSrcContext::Loc
FileSrcContext::File::add_line (FilePos bol_file_pos, FileOffs len)
{
  // See if there's an existing chunk for this position.
  //
  unsigned num_chunks = _chunks.size ();
  for (unsigned chunk_idx = 0; chunk_idx < num_chunks; chunk_idx++)
    {
      Chunk *existing_chunk = _chunks[chunk_idx];
      FilePos ex_beg_file_pos = existing_chunk->file_pos;
      FilePos ex_end_file_pos = existing_chunk->file_pos + existing_chunk->size;
      
      // If we're looking at a chunk which is afte BOL_FILE_POS, then
      // the caller is trying to add a line in the wrong order, so
      // throw an exception.
      //
      if (ex_beg_file_pos > bol_file_pos)
	throw std::runtime_error ("FileSrcContext line not added contiguously");

      // See if BOL_FILE_POS is _within_ EXISTING_CHUNK.
      //
      // In this case, BOL_FILE_POS + LEN must also fit within the current
      // bounds of that chunk plus any existing "extension" chunks.
      //
      if (ex_beg_file_pos <= bol_file_pos && ex_end_file_pos > bol_file_pos)
	{
	  // Increase EX_END_FILE_POS to include any "extension" chunks.
	  //
	  unsigned extension_num = 1;
	  while (chunk_idx + extension_num < num_chunks
		 && _chunks[chunk_idx + extension_num]->line_offs > 0)
	    {
	      ex_end_file_pos += _chunks[chunk_idx + extension_num]->size;
	      extension_num++;
	    }

	  // If this new line doesn't fit within the existing bound,
	  // that means the file has changed since we last saw it,
	  // which is a user error.
	  //
	  if (bol_file_pos + len > ex_end_file_pos)
	    throw std::runtime_error ("Source file line out of sync");

	  // Just return the info from the existing state.
	  //
	  return existing_chunk->loc + (bol_file_pos - ex_beg_file_pos);
	}

      // See if BOL_FILE_POS is precisely after EXISTING_CHUNK.
      //
      if (ex_beg_file_pos <= bol_file_pos && ex_end_file_pos == bol_file_pos)
	{
	  // See if EXISTING_CHUNK is the last chunk within the file,
	  // in which case we will either extend it, or add a new
	  // chunk after it.
	  //
	  if (chunk_idx == num_chunks - 1)
	    {
	      // If EXISTING_CHUNK can be extended, just do that.
	      //
	      if (existing_chunk->size + len <= MAX_CHUNK_SIZE)
		{
		  existing_chunk->size += len;

		  return existing_chunk->loc + (bol_file_pos - ex_beg_file_pos);
		}

	      // The global vector of chunks within our
	      // FileSrcContext.
	      //
	      std::vector<Chunk *> &global_chunks = _src_context._chunks;

	      // EXISTING_CHUNK can't be extended, because LEN is too
	      // long, but if it's the last global chunk, we can add
	      // "extension chunks" after it instead.
	      //
	      if (existing_chunk == global_chunks.back ())
		{
		  // Add extension chunks.

		  // Offset from beginning of line of each extension
		  // chunk.  This is initially for the first extension
		  // chunk, which starts after we use as much space in
		  // EXISTING_CHUNK as possible.
		  //
		  unsigned extension_chunk_line_offs
		    = (MAX_CHUNK_SIZE - existing_chunk->size);

		  // File position of each extension chunk.  This
		  // initially for the first extension chunk, which
		  // starts MAX_CHUNK_SIZE after EXISTING_CHUNK's file
		  // position (because we extend EXISTING_CHUNK to its
		  // max size).
		  //
		  FilePos extension_chunk_file_pos
		    = existing_chunk->file_pos + static_cast<FileOffs> (MAX_CHUNK_SIZE);

		  // First make EXISTING_CHUNK as long as possible.
		  //
		  len -= extension_chunk_line_offs;
		  existing_chunk->size = MAX_CHUNK_SIZE;

		  // Now add extension chunks until LEN is small
		  // enough to fit within the last one.
		  //
		  while (len > MAX_CHUNK_SIZE)
		    {
		      new Chunk (this, extension_chunk_file_pos,
				 MAX_CHUNK_SIZE,
				 extension_chunk_line_offs);

		      extension_chunk_line_offs += MAX_CHUNK_SIZE;
		      extension_chunk_file_pos += MAX_CHUNK_SIZE;
		      len -= MAX_CHUNK_SIZE;
		    }

		  // Now LEN is small enough that we can add a final
		  // extension chunk containing the end of the line.
		  //
		  new Chunk (this, extension_chunk_file_pos, len,
			     extension_chunk_line_offs);

		  // The final result is within the initial
		  // EXISTING_CHUNK (all the extension chunks we added
		  // essentially just serve to reserve the
		  // source-location namespace for the rest of the line).
		  //
		  return existing_chunk->loc + (bol_file_pos - ex_beg_file_pos);
		}
	    }
	}
    }

  // We couldn't extend any existing chunk, so just add a new chunk at
  // end of everything, which starts at the beginning of this line.
  //
  Chunk *new_chunk = new Chunk (this, bol_file_pos, len);

  // In the case of a new chunk
  return new_chunk->loc;
}


// Return the file chunk LOC is inside.
//
FileSrcContext::File::Chunk *
FileSrcContext::loc_chunk (Loc loc) const
{
  loc >>= LOC_OFFS_BITS;

  if (loc > _chunks.size ())
    throw std::runtime_error (
		 "Invalid location chunk number in FileSrcContext::loc_chunk");

  return _chunks[loc];
}



// Return info about the source-location LOC in the following
// out-parameters:
//  + FILE_NAME is the name of the source file LOC is from
//  + LINE_NUM is the line number of the line LOC is from in FILE_NAME
//  + LINE_OFFS is the offset of LOC in that line
//  + LINE_text is the actual text of the line
//
void
FileSrcContext::loc_source (Loc loc,
			    std::string &file_name,
			    unsigned &line_num, unsigned &line_offs,
			    std::string &line)
  const
{
  File::Chunk *chunk = loc_chunk (loc);

  file_name = chunk->file->name ();
  line_num = chunk->line_num;

  std::ifstream stream (file_name);

  stream.seekg (chunk->file_pos);

  Loc bol_loc = chunk->loc;
  for (;;)
    {
      if (! std::getline (stream, line))
	{
	  throw std::runtime_error
	    ("Source-location past end of file in FileSrcContext::loc_line");
	}

      unsigned line_len = line.size () + 1; // add 1 for the newline

      // If LINE now spans LOC, we've found the line we're looking
      // for, so break out of the loop.
      //
      if (bol_loc + line_len > loc)
	{
	  line_offs = loc - bol_loc;
	  break;
	}

      // Otherwise, adjust our numbers to include the effects of LINE
      // and keep looking.
      //
      bol_loc += line_len;
      line_num++;
    }
}


// Signal an exception for an error at location LOC within this
// context with message MSG.
//
[[noreturn]] void
FileSrcContext::error (Loc loc, const std::string &msg) const
{
  std::string file_name;
  std::string line;
  unsigned line_num;
  unsigned line_offs;

  loc_source (loc, file_name, line_num, line_offs, line);

  std::string desc (line);
  desc += '\n';
  for (unsigned i = 0; i < line_offs; i++)
    desc += ' ';
  desc += '^';
  desc += '\n';

  throw std::runtime_error (desc
			    + file_name
			    + ":" + std::to_string (line_num)
			    + ":" + std::to_string (line_offs)
			    + ": " + msg);
}
