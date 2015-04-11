CFLAGS = -g -std=c++0x -pthread

ARCH = x86_64
#ARCH = i386
LD = g++

ifeq  (${ARCH}, i386)
  $(info 'add -m32')
  CFLAGS += -m32
  ifeq  (${LD}, ld)
    LDFLAGS += -m elf_i386
  else
    LDFLAGS += -m32
  endif
else
  #$(info 'skip -m32')
endif

all: bst avl_test rb_tree

bst: bst.cpp basictreenode.cpp basictreenode.h
	g++ ${CFLAGS} bst.cpp basictreenode.cpp -o $@  ${LDFLAGS}
	
	
avl_test: avl_test.cpp avl_tree.cpp basictreenode.cpp basictreenode.h avl_tree.h
	g++ ${CFLAGS} avl_test.cpp avl_tree.cpp basictreenode.cpp -o $@  ${LDFLAGS}
	
rb_tree: rb_tree.cpp basictreenode.cpp basictreenode.h rb_tree.h
	g++ ${CFLAGS} rb_tree.cpp basictreenode.cpp -o $@  ${LDFLAGS}
	
memcheck: all
	valgrind --tool=memcheck --leak-check=yes --show-reachable=yes --num-callers=20 --track-fds=yes ./bst
	valgrind --tool=memcheck --leak-check=yes --show-reachable=yes --num-callers=20 --track-fds=yes ./rb_tree
	valgrind --tool=memcheck --leak-check=yes --show-reachable=yes --num-callers=20 --track-fds=yes ./avl_test	
	
.PHONY: clean

clean:
	rm bst avl_test