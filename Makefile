CXXFLAGS = -std=c++17 -pedantic-errors -Wall -Wextra -g -O3 -march=native

PROGS = compcat

all: $(PROGS)


OBJS = prog.o fun.o fun-opt.o fun-ssa.o bb.o bb-dom-tree.o		\
	insn.o cond-branch-insn.o calc-insn.o				\
	phi-fun-insn.o phi-fun-inp-insn.o				\
	reg.o value.o							\
	prog-text-writer.o						\
	fun-text-writer.o bb-text-writer.o insn-text-writer.o		\
	fun-text-reader.o fun-text-reader-parse-fun.o			\
	line-input.o prog-text-reader.o					\
	file-input.o file-src-context.o					\
	check-assertion.o

compcat: compcat.o $(OBJS)
	$(CXX) -o $@ $(CXXFLAGS) $^


# Include file dependencies, which should be transitively used by
# dependent source files.
#
calc-insn.h-DEPS = insn.h $(insn.h-DEPS)
cond-branch-insn.h-DEPS = insn.h $(insn.h-DEPS)
copy-insn.h-DEPS = insn.h $(insn.h-DEPS)
file-input.h-DEPS = file-src-context.h $(file-src-context.h-DEPS)
file-src-context.h-DEPS = src-context.h $(src-context.h-DEPS)
fun-arg-insn.h-DEPS = insn.h $(insn.h-DEPS)
fun-result-insn.h-DEPS = insn.h $(insn.h-DEPS)
fun-text-writer.h-DEPS = insn-text-writer.h $(insn-text-writer.h-DEPS) \
    bb-text-writer.h $(bb-text-writer.h-DEPS)
fun.h-DEPS = bb.h $(bb.h-DEPS)
nop-insn.h-DEPS = insn.h $(insn.h-DEPS)
phi-fun-inp-insn.h-DEPS = insn.h $(insn.h-DEPS)
phi-fun-insn.h-DEPS = insn.h $(insn.h-DEPS)
prog-text-reader.h-DEPS = fun-text-reader.h $(fun-text-reader.h-DEPS)
prog-text-writer.h-DEPS = fun-text-writer.h $(fun-text-writer.h-DEPS)
prog.h-DEPS = fun.h $(fun.h-DEPS)


# Object file dependencies, basically the corresponding source file
# plus any include files it uses.
#
bb-dom-tree.o: bb-dom-tree.cc                       \
    bb.h $(bb.h-DEPS)
bb-text-writer.o: bb-text-writer.cc                 \
    fun.h $(fun.h-DEPS)                             \
    bb.h $(bb.h-DEPS)                               \
    fun-text-writer.h $(fun-text-writer.h-DEPS)     \
    insn-text-writer.h $(insn-text-writer.h-DEPS)   \
    bb-text-writer.h $(bb-text-writer.h-DEPS)
bb.o: bb.cc                                         \
    check-assertion.h $(check-assertion.h-DEPS)     \
    bb.h $(bb.h-DEPS)                               \
    insn.h $(insn.h-DEPS)                           \
    remove-one.h $(remove-one.h-DEPS)               \
    fun.h $(fun.h-DEPS)
bitvec.o: bitvec.cc                                 \
    bitvec.h $(bitvec.h-DEPS)
calc-insn.o: calc-insn.cc                           \
    calc-insn.h $(calc-insn.h-DEPS)
check-assertion.o: check-assertion.cc               \
    check-assertion.h $(check-assertion.h-DEPS)
compcat.o: compcat.cc                               \
    fun.h $(fun.h-DEPS)                             \
    prog.h $(prog.h-DEPS)                           \
    prog-text-writer.h $(prog-text-writer.h-DEPS)   \
    line-input.h $(line-input.h-DEPS)               \
    prog-text-reader.h $(prog-text-reader.h-DEPS)
cond-branch-insn.o: cond-branch-insn.cc             \
    check-assertion.h $(check-assertion.h-DEPS)     \
    bb.h $(bb.h-DEPS)                               \
    reg.h $(reg.h-DEPS)                             \
    cond-branch-insn.h $(cond-branch-insn.h-DEPS)
file-input.o: file-input.cc                         \
    file-input.h $(file-input.h-DEPS)
file-src-context.o: file-src-context.cc             \
    check-assertion.h $(check-assertion.h-DEPS)     \
    file-src-context.h $(file-src-context.h-DEPS)
fun-opt.o: fun-opt.cc                               \
    check-assertion.h $(check-assertion.h-DEPS)     \
    insn.h $(insn.h-DEPS)                           \
    fun.h $(fun.h-DEPS)
fun-ssa.o: fun-ssa.cc                               \
    check-assertion.h $(check-assertion.h-DEPS)     \
    reg.h $(reg.h-DEPS)                             \
    insn.h $(insn.h-DEPS)                           \
    copy-insn.h $(copy-insn.h-DEPS)                 \
    phi-fun-insn.h $(phi-fun-insn.h-DEPS)           \
    phi-fun-inp-insn.h $(phi-fun-inp-insn.h-DEPS)   \
    fun.h $(fun.h-DEPS)
fun-text-reader-parse-fun.o: fun-text-reader-parse-fun.cc \
    fun.h $(fun.h-DEPS)                             \
    reg.h $(reg.h-DEPS)                             \
    cond-branch-insn.h $(cond-branch-insn.h-DEPS)   \
    nop-insn.h $(nop-insn.h-DEPS)                   \
    calc-insn.h $(calc-insn.h-DEPS)                 \
    copy-insn.h $(copy-insn.h-DEPS)                 \
    fun-arg-insn.h $(fun-arg-insn.h-DEPS)           \
    fun-result-insn.h $(fun-result-insn.h-DEPS)     \
    line-input.h $(line-input.h-DEPS)               \
    fun-text-reader.h $(fun-text-reader.h-DEPS)
fun-text-reader.o: fun-text-reader.cc               \
    fun.h $(fun.h-DEPS)                             \
    reg.h $(reg.h-DEPS)                             \
    value.h $(value.h-DEPS)                         \
    cond-branch-insn.h $(cond-branch-insn.h-DEPS)   \
    nop-insn.h $(nop-insn.h-DEPS)                   \
    calc-insn.h $(calc-insn.h-DEPS)                 \
    copy-insn.h $(copy-insn.h-DEPS)                 \
    fun-arg-insn.h $(fun-arg-insn.h-DEPS)           \
    fun-result-insn.h $(fun-result-insn.h-DEPS)     \
    line-input.h $(line-input.h-DEPS)               \
    prog-text-reader.h $(prog-text-reader.h-DEPS)   \
    fun-text-reader.h $(fun-text-reader.h-DEPS)
fun-text-writer.o: fun-text-writer.cc               \
    reg.h $(reg.h-DEPS)                             \
    fun.h $(fun.h-DEPS)                             \
    value.h $(value.h-DEPS)                         \
    prog-text-writer.h $(prog-text-writer.h-DEPS)   \
    fun-text-writer.h $(fun-text-writer.h-DEPS)
fun.o: fun.cc                                       \
    reg.h $(reg.h-DEPS)                             \
    fun.h $(fun.h-DEPS)
insn-text-writer.o: insn-text-writer.cc             \
    bb.h $(bb.h-DEPS)                               \
    reg.h $(reg.h-DEPS)                             \
    value.h $(value.h-DEPS)                         \
    cond-branch-insn.h $(cond-branch-insn.h-DEPS)   \
    nop-insn.h $(nop-insn.h-DEPS)                   \
    calc-insn.h $(calc-insn.h-DEPS)                 \
    copy-insn.h $(copy-insn.h-DEPS)                 \
    fun-arg-insn.h $(fun-arg-insn.h-DEPS)           \
    fun-result-insn.h $(fun-result-insn.h-DEPS)     \
    phi-fun-insn.h $(phi-fun-insn.h-DEPS)           \
    phi-fun-inp-insn.h $(phi-fun-inp-insn.h-DEPS)   \
    fun-text-writer.h $(fun-text-writer.h-DEPS)     \
    insn-text-writer.h $(insn-text-writer.h-DEPS)
insn.o: insn.cc                                     \
    check-assertion.h $(check-assertion.h-DEPS)     \
    bb.h $(bb.h-DEPS)                               \
    reg.h $(reg.h-DEPS)                             \
    insn.h $(insn.h-DEPS)
line-input.o: line-input.cc                         \
    line-input.h $(line-input.h-DEPS)
phi-fun-inp-insn.o: phi-fun-inp-insn.cc             \
    bb.h $(bb.h-DEPS)                               \
    phi-fun-insn.h $(phi-fun-insn.h-DEPS)           \
    phi-fun-inp-insn.h $(phi-fun-inp-insn.h-DEPS)
phi-fun-insn.o: phi-fun-insn.cc                     \
    bb.h $(bb.h-DEPS)                               \
    reg.h $(reg.h-DEPS)                             \
    phi-fun-inp-insn.h $(phi-fun-inp-insn.h-DEPS)   \
    phi-fun-insn.h $(phi-fun-insn.h-DEPS)
prog-text-reader.o: prog-text-reader.cc             \
    prog.h $(prog.h-DEPS)                           \
    line-input.h $(line-input.h-DEPS)               \
    prog-text-reader.h $(prog-text-reader.h-DEPS)
prog-text-writer.o: prog-text-writer.cc             \
    prog.h $(prog.h-DEPS)                           \
    prog-text-writer.h $(prog-text-writer.h-DEPS)
prog.o: prog.cc                                     \
    fun.h $(fun.h-DEPS)                             \
    prog.h $(prog.h-DEPS)
reg.o: reg.cc                                       \
    fun.h $(fun.h-DEPS)                             \
    insn.h $(insn.h-DEPS)                           \
    value.h $(value.h-DEPS)                         \
    reg.h $(reg.h-DEPS)
value.o: value.cc                                   \
    fun.h $(fun.h-DEPS)                             \
    insn.h $(insn.h-DEPS)                           \
    value.h $(value.h-DEPS)


clean:
	rm -f $(PROGS) *.o
