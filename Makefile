CXXFLAGS = -std=c++11 -pedantic-errors -Wall -Wextra -g -O3 -march=native

PROGS = compcat

all: $(PROGS)


OBJS = prog.o fun.o bb.o bb.o bb-dom-tree.o insn.o cond-branch-insn.o	\
	prog-text-writer.o						\
	fun-text-writer.o bb-text-writer.o insn-text-writer.o		\
	text-reader-inp.o prog-text-reader.o fun-text-reader.o		\
	check-assertion.o

compcat: compcat.o $(OBJS)
	$(CXX) -o $@ $(CXXFLAGS) $^


#
# Include file dependencies, which should be transitively used by
# dependent source files.
#

prog.h-DEPS = fun.h $(fun.h-DEPS)
fun.h-DEPS = bb.h $(bb.h-DEPS)
bb.h-DEPS = memtoobj.h $(memtoobj.h-DEPS)
insn.h-DEPS = 
cond-branch-insn.h-DEPS = insn.h $(insn.h-DEPS)

prog-text-writer.h-DEPS = fun-text-writer.h $(fun-text-writer.h-DEPS)
fun-text-writer.h-DEPS = insn-text-writer.h $(insn-text-writer.h-DEPS)	\
	bb-text-writer.h $(bb-text-writer.h-DEPS)
bb-text-writer.h-DEPS =
insn-text-writer.h-DEPS = 

text-reader-inp.h-DEPS = 
fun-text-reader.h-DEPS = 


#
# Object file dependencies, basically the corresponding source file
# plus any include files it uses.
#

prog.o: prog.cc prog.h $(prog.h-DEPS)
fun.o: fun.cc fun.h $(fun.h-DEPS)
bb.o: bb.cc bb.h $(bb.h-DEPS) insn.h $(insn.h-DEPS)
bb-dom-tree.o: bb-dom-tree.cc bb.h $(bb.h-DEPS)
insn.o: insn.cc insn.h $(insn.h-DEPS) bb.h $(bb.h-DEPS)
cond-branch-insn.o: cond-branch-insn.cc			\
	cond-branch-insn.h $(cond-branch-insn.h-DEPS)	\
	bb.h $(bb.h-DEPS)

prog-text-writer.o: prog-text-writer.cc			\
	prog-text-writer.h $(prog-text-writer.h-DEPS)
fun-text-writer.o: fun-text-writer.cc			\
	fun-text-writer.h $(fun-text-writer.h-DEPS)	\
	fun.h $(fun.h-DEPS) reg.h $(reg.h-DEPS)
bb-text-writer.o: bb-text-writer.cc			\
	bb-text-writer.h $(bb-text-writer.h-DEPS)	\
	insn-text-writer.h $(insn-text-writer.h-DEPS)	\
	fun-text-writer.h $(fun-text-writer.h-DEPS)	\
	fun.h $(fun.h-DEPS) bb.h $(bb.h-DEPS)
insn-text-writer.o: insn-text-writer.cc			\
	insn-text-writer.h $(insn-text-writer.h-DEPS)	\
	fun-text-writer.h $(fun-text-writer.h-DEPS)	\
	nop-insn.h $(nop-insn.h-DEPS)			\
	cond-branch-insn.h $(cond-branch-insn.h-DEPS)	\
	reg.h $(reg.h-DEPS) bb.h $(bb.h-DEPS)

text-reader-inp.o: text-reader-inp.cc			\
	text-reader-inp.h $(text-reader-inp.h-DEPS)
prog-text-reader.o: prog-text-reader.cc			\
	prog.h $(prog.h-DEPS)				\
	text-reader-inp.h $(text-reader-inp.h-DEPS)	\
	prog-text-reader.h $(prog-text-reader.h-DEPS)
fun-text-reader.o: fun-text-reader.cc			\
	fun-text-reader.h $(fun-text-reader.h-DEPS)	\
	fun.h $(fun.h-DEPS)				\
	cond-branch-insn.h $(cond-branch-insn.h-DEPS)	\
	nop-insn.h $(nop-insn.h-DEPS)

check-assertion.o: check-assertion.cc			\
	check-assertion.h $(check-assertion.h-DEPS)

compcat.o: compcat.cc					\
	fun.h $(fun.h-DEPS)				\
	prog-text-writer.h $(prog-text-writer.h-DEPS)	\
	text-reader-inp.h $(text-reader-inp.h-DEPS)	\
	prog-text-reader.h $(prog-text-reader.h-DEPS)

bitvec.o: bitvec.cc bitvec.h $(bitvec.h-DEPS)

clean:
	rm -f $(PROGS) *.o
