#include <iostream>
#include <memory>

#include "fun.h"
#include "prog.h"
#include "prog-text-writer.h"

#include "file-src-context.h"
#include "src-file-input.h"
#include "prog-text-reader.h"

int main (int argc, const char *const *argv)
{
  if (argc != 2)
    {
      std::cerr << "Usage: " << argv[0] << " SRC_FILE\n";
      return 1;
    }

  try
    {
      FileSrcContext src_context;
      SrcFileInput inp (argv[1], src_context);
      ProgTextReader prog_reader (inp);
      std::unique_ptr<Prog> prog (prog_reader.read ());

      for (auto [_, fun] : prog->functions ())
	{
	  fun->combine_blocks ();
	  fun->remove_unreachable ();

	  fun->update_dominators ();
	  fun->update_post_dominators ();

	  fun->convert_to_ssa_form ();
	  fun->propagate_through_copies ();
	  fun->convert_from_ssa_form ();

	  fun->remove_useless_copies ();
	}

      ProgTextWriter prog_writer (std::cout);
      prog_writer.write (&*prog);
    }
  catch (std::runtime_error &err)
    {
      std::cerr << err.what () << '\n';
    }

  return 0;
}
