CXXFLAGS = -std=c++11 -pedantic-errors -Wall -Wextra -g -O3 -march=native

PROGS = comp

all: $(PROGS)


OBJS = flow-graph.o bb.o bb-dom-tree.o insn.o cond-branch-insn.o \
	flow-graph-io-dumper.o bb-io-dumper.o insn-io-dumper.o 

comp: $(OBJS)


# Include file dependencies, which should be transitively used by
# dependent source files.
#
bb.h-DEPS = memtoobj.h $(memtoobj.h-DEPS)
flow-graph.h-DEPS = bb.h $(bb.h-DEPS)
cond-branch-insn.h-DEPS = insn.h $(insn.h-DEPS)

flow-graph-io.h-DEPS = insn-io.h $(insn-io.h-DEPS) bb-io.h $(bb-io.h-DEPS)

# Object file dependencies, basically the corresponding source file
# plus any include files it uses.
#
flow-graph.o: flow-graph.cc flow-graph.h $(flow-graph.h-DEPS)
bb.o: bb.cc bb.h $(bb.h-DEPS) insn.h $(insn.h-DEPS)
bb-dom-tree.o: bb-dom-tree.cc bb.h $(bb.h-DEPS)
insn.o: insn.cc insn.h $(insn.h-DEPS) bb.h $(bb.h-DEPS)
cond-branch-insn.o: cond-branch-insn.cc			\
	cond-branch-insn.h $(cond-branch-insn.h-DEPS)	\
	bb.h $(bb.h-DEPS)

flow-graph-io-dumper.o: flow-graph-io-dumper.cc		\
	flow-graph-io.h $(flow-graph-io.h-DEPS)		\
	flow-graph.h $(flow-graph.h-DEPS)		\
	cond-branch-insn.h $(cond-branch-insn.h-DEPS)	\
	bb.h $(bb.h-DEPS)
bb-io-dumper.o: bb-io-dumper.cc bb-io.h $(bb-io.h-DEPS) \
	insn-io.h $(insn-io.h-DEPS) flow-graph-io.h $(flow-graph-io.h-DEPS)
insn-io-dumper.o: insn-io-dumper.cc insn-io.h $(insn-io.h-DEPS)	\
	flow-graph-io.h $(flow-graph-io.h-DEPS)

bitvec.o: bitvec.cc bitvec.h $(bitvec.h-DEPS)


clean:
	rm -f $(PROGS) *.o
