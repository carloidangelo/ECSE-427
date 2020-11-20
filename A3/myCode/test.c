/*
 ============================================================================
 Name        : myHello.c
 Author      : carlo
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>


int main(int argc, char *argv[]) {
    int i, sum, user_input; // Get user input
	printf("Program to computer sum of integers from 0 to N\n");
	printf("Enter series limit: ");
	scanf("%d",&user_input);
	printf("Computing the sum of digits from 0 to %d\n",user_input);
	// Do the math

	sum=0;
	for (i=1;i<=user_input;i++) {
		sum=sum+i;// sum+=i;
	}
	// Produce answer and end program

	printf("Result = %d\n\n",sum);
	int *p;
	int *r;
	int x[5] = {30,-3,12,45,-10};
	int y;
	void *q;
	unsigned long z, t;

	printf("%lu\n",sizeof(unsigned long));
	p = &x[1];
	q = (void *)p;
	y = *p;
	z = (unsigned long)p;
	t = (unsigned long)q;
	printf("p = %p\n", p);
	printf("y = %d\n", y);
	printf("z = %lu\n", z);
	printf("t = %lu\n", t);
	r = p + 2;
	y = *p;
	printf("r = %p\n", r);
	printf("y = %d\n", y);
	printf("test = %d\n", p == q);
	char* msg = "-4";
	printf("string = %d\n", atoi(msg));
	return 0;
}