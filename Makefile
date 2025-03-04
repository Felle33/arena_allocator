CFLAGS=-Wall -Werror -ggdb

test1: test1.c arena.c arena.h
	gcc $(CFLAGS) test1.c arena.c arena.h -o build/test1
