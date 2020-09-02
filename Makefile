#
# Makefile for smartstr, uses clang to produce a static library.
#
# You do not have to use this, you might just as well import the tiny
# smartstr.c module right into your own project.
#
# https://github.com/mateuszviste/smartstr
#

CC = clang
CFLAGS = -Wall -pedantic -std=gnu89 -Wextra -Weverything -O2 -Wno-padded

all: smartstr.a test

smartstr.a: smartstr.o
	ar rcs smartstr.a smartstr.o

test: test.c smartstr.a

clean:
	rm -f test *.o *.a
