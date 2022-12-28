CC = gcc
LD = -lreadline
CCFLAGS =

SRC = $(wildcard *.c)
TST_SRC = axioms.c data.c pointer.c repl.c test/ptr_test.c
all:
	$(CC) -Wall -ggdb $(SRC) $(LD) -o lisp

test:
	$(CC) -Wall -ggdb $(TST_SRC) -o test/test

format:
	find . -regex '.*\.\(c\|h\)' -exec clang-format -style=file -i {} \;
