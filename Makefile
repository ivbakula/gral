CC = gcc
LD = -lreadline
CCFLAGS =

SRC = $(wildcard *.c)

all:
	$(CC) -Wall -ggdb $(SRC) $(LD) -o lisp
