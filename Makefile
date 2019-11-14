CXXFLAGS = -std=c++11 -pedantic-errors -Wall -Wextra -g -O3 -march=native

PROGS = comp

all: $(PROGS)


OBJS = fun.o bb.o bb.o bb-dom-tree.o insn.o cond-branch-insn.o	\
	fun-text-writer.o bb-text-writer.o insn-text-writer.o	\
	text-reader-inp.o						\
	fun-text-reader.o insn-text-reader.o bb-text-reader.o		\
	check-assertion.o

comp: $(OBJS)


#
# Include file dependencies, which should be transitively used by
# dependent source files.
#

fun.h-DEPS = bb.h $(bb.h-DEPS)
bb.h-DEPS = memtoobj.h $(memtoobj.h-DEPS)
insn.h-DEPS = 
cond-branch-insn.h-DEPS = insn.h $(insn.h-DEPS)

fun-text-writer.h-DEPS = insn-text-writer.h $(insn-text-writer.h-DEPS)	\
	bb-text-writer.h $(bb-text-writer.h-DEPS)
bb-text-writer.h-DEPS =
insn-text-writer.h-DEPS = 

text-reader-inp.h-DEPS = 
fun-text-reader.h-DEPS = insn-text-reader.h $(insn-text-reader.h-DEPS) \
	bb-text-reader.h $(bb-text-reader.h-DEPS)
bb-text-reader.h-DEPS =
insn-text-reader.h-DEPS = 


#
# Object file dependencies, basically the corresponding source file
# plus any include files it uses.
#

fun.o: fun.cc fun.h $(fun.h-DEPS)
bb.o: bb.cc bb.h $(bb.h-DEPS) insn.h $(insn.h-DEPS)
bb-dom-tree.o: bb-dom-tree.cc bb.h $(bb.h-DEPS)
insn.o: insn.cc insn.h $(insn.h-DEPS) bb.h $(bb.h-DEPS)
cond-branch-insn.o: cond-branch-insn.cc			\
	cond-branch-insn.h $(cond-branch-insn.h-DEPS)	\
	bb.h $(bb.h-DEPS)

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
	cond-branch-insn.h $(cond-branch-insn.h-DEPS)	\
	reg.h $(reg.h-DEPS) bb.h $(bb.h-DEPS)

text-reader-inp.o: text-reader-inp.cc			\
	text-reader-inp.h $(text-reader-inp.h-DEPS)
fun-text-reader.o: fun-text-reader.cc			\
	fun-text-reader.h $(fun-text-reader.h-DEPS)	\
	fun.h $(fun.h-DEPS)				\
	cond-branch-insn.h $(cond-branch-insn.h-DEPS)
bb-text-reader.o: bb-text-reader.cc			\
	bb-text-reader.h $(bb-text-reader.h-DEPS)	\
	insn-text-reader.h $(insn-text-reader.h-DEPS)	\
	fun-text-reader.h $(fun-text-reader.h-DEPS)
insn-text-reader.o: insn-text-reader.cc			\
	insn-text-reader.h $(insn-text-reader.h-DEPS)	\
	fun-text-reader.h $(fun-text-reader.h-DEPS)	\
	cond-branch-insn.h $(cond-branch-insn.h-DEPS)	\
	reg.h $(reg.h-DEPS) bb.h $(bb.h-DEPS)

check-assertion.o: check-assertion.cc			\
	check-assertion.h $(check-assertion.h-DEPS)

bitvec.o: bitvec.cc bitvec.h $(bitvec.h-DEPS)

clean:
	rm -f $(PROGS) *.o
