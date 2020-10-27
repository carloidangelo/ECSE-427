# .DEFAULT_GOAL=all

CC=gcc
CFLAGS=-fsanitize=signed-integer-overflow -pthread -fsanitize=undefined -g -std=gnu99 -O2 -Wall -Wextra -Wno-sign-compare -Wno-unused-parameter -Wno-unused-variable -Wshadow

TEST1=test1
TEST2=test2
TEST3=test3
TEST4=test4

SERVER=a2_server

server: a1_lib.c example_server.c
	$(CC) -o $(SERVER) $(CFLAGS) example_server.c a1_lib.c

test: test1.c test2.c test3.c test4.c sut.c a1_lib.c
	$(CC) -o $(TEST1) $(CFLAGS) test1.c sut.c a1_lib.c
	$(CC) -o $(TEST2) $(CFLAGS) test2.c sut.c a1_lib.c
	$(CC) -o $(TEST3) $(CFLAGS) test3.c sut.c a1_lib.c
	$(CC) -o $(TEST4) $(CFLAGS) test4.c sut.c a1_lib.c

clean:
	rm -rf $(SERVER)
	rm -rf $(TEST1)
	rm -rf $(TEST2)
	rm -rf $(TEST3)
	rm -rf $(TEST4)

