# .DEFAULT_GOAL=all

CC=gcc
BINARY_SUT=sut.o
BINARY_LIB=a1_lib.o

all_binary: sut.c a1_lib.c
	$(CC) -o $(BINARY_SUT) -c sut.c
	$(CC) -o $(BINARY_LIB) -c a1_lib.c

clean:
	rm -rf $(BINARY_SUT)
	rm -rf $(BINARY_LIB)

