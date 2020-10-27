#ifndef __THREAD__
#define __THREAD__

#include <ucontext.h>
#define MAX_THREADS                        32
#define THREAD_STACK_SIZE                  1024*64

typedef struct __threaddesc
{
	int threadid;
	char *threadstack;
	void *threadfunc;
	ucontext_t threadcontext;
} threaddesc;

#endif