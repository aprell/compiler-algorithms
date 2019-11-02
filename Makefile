CXXFLAGS = -std=c++11 -pedantic-errors -Wall -Wextra -g -O3 -march=native

PROGS = comp

all: $(PROGS)


OBJS = bb.o bb-dom-tree.o insn.o cond-branch-insn.o	\
	fun-io-dumper.o bb-io-dumper.o insn-io-dumper.o

comp: $(OBJS)


# Include file dependencies, which should be transitively used by
# dependent source files.
#
bb.h-DEPS = memtoobj.h $(memtoobj.h-DEPS)
fun.h-DEPS = bb.h $(bb.h-DEPS)
cond-branch-insn.h-DEPS = insn.h $(insn.h-DEPS)

fun-io.h-DEPS = insn-io.h $(insn-io.h-DEPS) bb-io.h $(bb-io.h-DEPS)

# Object file dependencies, basically the corresponding source file
# plus any include files it uses.
#
bb.o: bb.cc bb.h $(bb.h-DEPS) insn.h $(insn.h-DEPS)
bb-dom-tree.o: bb-dom-tree.cc bb.h $(bb.h-DEPS)
insn.o: insn.cc insn.h $(insn.h-DEPS) bb.h $(bb.h-DEPS)
cond-branch-insn.o: cond-branch-insn.cc			\
	cond-branch-insn.h $(cond-branch-insn.h-DEPS)	\
	bb.h $(bb.h-DEPS)

fun-io-dumper.o: fun-io-dumper.cc		\
	fun-io.h $(fun-io.h-DEPS)		\
	fun.h $(fun.h-DEPS)		\
	reg.h $(reg.h-DEPS)
bb-io-dumper.o: bb-io-dumper.cc bb-io.h $(bb-io.h-DEPS) \
	insn-io.h $(insn-io.h-DEPS) fun-io.h $(fun-io.h-DEPS)
insn-io-dumper.o: insn-io-dumper.cc insn-io.h $(insn-io.h-DEPS)	\
	fun-io.h $(fun-io.h-DEPS)			\
	cond-branch-insn.h $(cond-branch-insn.h-DEPS)		\
	reg.h $(reg.h-DEPS) bb.h $(bb.h-DEPS)

bitvec.o: bitvec.cc bitvec.h $(bitvec.h-DEPS)


clean:
	rm -f $(PROGS) *.o
