# .DEFAULT_GOAL=all

CC=gcc
CFLAGS=-fsanitize=signed-integer-overflow -pthread -fsanitize=undefined -g -std=gnu99 -O2 -Wall -Wextra -Wno-sign-compare -Wno-unused-parameter -Wno-unused-variable -Wshadow

THREAD=test1

test: test1.c sut.c 
	$(CC) -o $(THREAD) $(CFLAGS) test1.c sut.c

clean:
	rm -rf $(THREAD)

