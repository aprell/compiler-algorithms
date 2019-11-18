#include <iostream>
#include <memory>

#include "fun.h"
#include "prog.h"
#include "prog-text-writer.h"

#include "text-reader-inp.h"
#include "prog-text-reader.h"

int main ()
{
  try
    {
      TextReaderInp inp (std::cin);
      ProgTextReader prog_reader (inp);
      std::unique_ptr<Prog> prog (prog_reader.read ());

      for (auto [_, fun] : prog->functions ())
	{
	  fun->combine_blocks ();
	  fun->remove_unreachable ();

	  fun->update_dominators ();
	  fun->update_post_dominators ();
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
