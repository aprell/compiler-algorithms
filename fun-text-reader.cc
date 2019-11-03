#include <stdexcept>

#include "fun.h"
#include "reg.h"

#include "cond-branch-insn.h"

#include "fun-text-reader.h"


FunTextReader::FunTextReader (std::istream &_in)
  : in (_in), insn_reader (*this), block_reader (*this)
{
}


// Read a text representation of a function, and return the new function..
//
Fun *
FunTextReader::read ()
{
  // Cannot be called recursively.
  //
  if (cur_fun)
    throw std::runtime_error ("Recursive call to FunTextReader::read");

  Fun *fun = new Fun ();

  cur_fun = fun;
  parse_fun ();
  cur_fun = 0;

  // Clear any left over parsing state.
  //
  clear_state ();

  return fun;
}


// Parse the contents of a function.
//
void
FunTextReader::parse_fun ()
{
  read_line ();
  expect ('{');

  bool first_block = true;

  while (read_line ())
    {
      if (skip ('}'))
	break;

      unsigned len = cur_line.length ();
      while (len > 0 && is_whitespace (cur_line[len - 1]))
	len--;

      // Comment line
      //
      if (skip ('#'))
	continue;

      // Label (starts a new block)
      //
      if (len > 0 && cur_line[len - 1] == ':')
	{
	  BB *prev_block = cur_block;

	  std::string lab_id = read_id ();
	  cur_block = label_block (lab_id);
	  //	  cur_block = read_label ();
	  expect (':');

	  if (! cur_block->is_empty ())
	    parse_error (std::string ("duplicate label _") + std::to_string (cur_block->num ()));

	  if (prev_block)
	    prev_block->set_fall_through (cur_block);

	  if (first_block)
	    {
	      cur_fun->set_entry_block (cur_block);
	      first_block = false;
	    }

	  continue;
	}

      // All other command forms start with an ID, which may be a
      // register name or a command name.
      //
      std::string id = read_id ();

      // Register declaration
      //
      if (id == "reg" && peek () != ':')
	{
	  std::string reg_name = read_id ();

	  Reg *&reg = registers[reg_name];
	  if (reg)
	    parse_error (std::string ("Duplicate register declaration \"") + id + "\"");
	  reg = new Reg (reg_name);

	  cur_fun->add_reg (reg);

	  continue;
	}

      // For everything after this point, a block is expected.
      //
      if (! cur_block)
	parse_error ("Expected label");

      // Assignment, of the form "REG := ..."
      //
      if (peek () == ':')
	{
	  continue;
	}

      // Branch insn, ends the current block
      //
      if (id == "goto")
	{
	  BB *target = read_label ();
	  cur_block->set_fall_through (target);
	  cur_block = 0;

	  continue;
	}

      // Conditional branch insn
      //
      if (id == "if")
	{
	  expect ('(');
	  Reg *cond = read_reg ();
	  expect (')');
	  expect ("goto");

	  BB *target = read_label ();

	  new CondBranchInsn (cond, target, cur_block);

	  continue;
	}
    }

  // If control continues to the end of the function, add an exit
  // block.
  //
  if (cur_block)
    {
      // If the current block isn't suitable for use as an exit block,
      // add a new block following it.
      //
      if (! cur_block->is_empty () || ! cur_block->successors ().empty ())
	{
	  BB *last_user_block = cur_block;
	  cur_block = new BB (cur_fun);
	  last_user_block->set_fall_through (cur_block);
	}

      cur_fun->set_exit_block (cur_block);
    }
}


// Read a new line, and return true if successful.
//
bool
FunTextReader::read_line ()
{
  if (std::getline (in, cur_line))
    {
      cur_line_offs = 0;
      return true;
    }
  else
    return false;
}


// If the character CH is the next unread character, skip it,
// otherwise signal an error.
//
void
FunTextReader::expect (char ch)
{
  if (! skip (ch))
    parse_error (std::string ("Expected '") + ch + "'");
}

// If the keyword KEYW follows the current position, skip it, 
// otherwise signal an error.
//
void
FunTextReader::expect (const char *keyw)
{
  if (! skip (keyw))
    parse_error (std::string ("Expected \"") + keyw + "\"");
}


// If the character CH is the next unread character, skip it, and
// return true, otherwise return false.
//
bool
FunTextReader::skip (char ch)
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
FunTextReader::skip (const char *keyw)
{
  skip_whitespace ();

  unsigned keyw_len = strlen (keyw);

  if (cur_line_offs + keyw_len > cur_line.length ())
    return false;

  if (is_id_cont_char (cur_line[cur_line_offs + keyw_len]))
    return false;

  for (unsigned i = 0; i < keyw_len; i++)
    if (cur_line[cur_line_offs + i] != keyw[i])
      return false;

  cur_line_offs += keyw_len;

  return true;
}


// Read and return an unsigned integer, or signal an error if none
// can be read.
//
unsigned
FunTextReader::read_unsigned ()
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
FunTextReader::read_id ()
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


// Read a register (which must exist) .
//
Reg *
FunTextReader::read_reg ()
{
  std::string reg_name = read_id ();

  auto reg_it = registers.find (reg_name);
  if (reg_it == registers.end ())
    parse_error (std::string ("Unknown register \"") + reg_name + "\"");

  return reg_it->second;
}


// Throw an exception containing the error message ERR.
//
void
FunTextReader::parse_error (const std::string &err) const
{
  throw std::runtime_error (cur_line_parse_desc () + "Parse error: " + err);
}


// Return a string showing the current line and parse position.
//
std::string
FunTextReader::cur_line_parse_desc () const
{
  std::string desc (cur_line);
  desc += '\n';
  for (unsigned i = 0; i < cur_line_offs; i++)
    desc += ' ';
  desc += '^';
  desc += '\n';
  return desc;
}


// Clear any parsing state used while parsing a function.
//
void
FunTextReader::clear_state ()
{
  labeled_blocks.clear ();
}


// Return the block corresponding to the label LABEL.  If no such
// label has yet been encountered, a new block is added and returned.
//
BB *
FunTextReader::label_block (const std::string &label)
{
  BB *&block_ptr = labeled_blocks[label];

  if (! block_ptr)
    block_ptr = new BB (cur_fun);

  return block_ptr;
}
