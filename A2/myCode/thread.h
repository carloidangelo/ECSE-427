#ifndef __THREAD__
#define __THREAD__

#include <ucontext.h>
#define MAX_EXEC                           2
#define MAX_THREADS                        32
#define THREAD_STACK_SIZE                  1024*64

#define RR                                 1   // round robin
#define FCFS                               2   // first come first served

#define RR_QUANTUM                         2   // in seconds

typedef struct __threaddesc
{
	int threadid;
	char *threadstack;
	void *threadfunc;
	ucontext_t threadcontext;
} threaddesc;

extern threaddesc threadarr[MAX_THREADS];
extern int numthreads, numthreadscancelled;

#endif