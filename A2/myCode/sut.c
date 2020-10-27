#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <time.h>
#include "queue.h" 
#include "thread.h"                                                                           
#include "sut.h"

struct queue q;
pthread_t tid_c_exec;
ucontext_t c_exec_context;
threaddesc threadarr[MAX_THREADS];
int numthreads, numthreadscancelled;

void *c_exec()
{
	struct queue_entry *elem;
	while(1){
		elem = queue_peek_front(&q);
		if (elem == NULL && numthreads > 0 && numthreads-numthreadscancelled == 0){
			pthread_exit(0);
		}else if (elem == NULL){
			struct timespec tim, tim2;
   			tim.tv_sec = 0;
   			tim.tv_nsec = 100 * 1000;
			nanosleep(&tim,&tim2);
		}else{
			swapcontext(&c_exec_context, &(elem->thread->threadcontext));
		}
	}
}

void sut_init()
{
    q = queue_create();
    queue_init(&q);
	numthreads = 0;
	numthreadscancelled = 0;
	pthread_create(&tid_c_exec, NULL, c_exec, NULL);
}

bool sut_create(sut_task_f fn)
{
	threaddesc *tdescptr;

	if (numthreads >= 32) 
	{
		printf("FATAL: Maximum thread limit reached... creation failed! \n");
		return false;
	}

	tdescptr = &(threadarr[numthreads]);
	getcontext(&(tdescptr->threadcontext));
	tdescptr->threadid = numthreads;
	tdescptr->threadstack = (char *)malloc(THREAD_STACK_SIZE);
	tdescptr->threadcontext.uc_stack.ss_sp = tdescptr->threadstack;
	tdescptr->threadcontext.uc_stack.ss_size = THREAD_STACK_SIZE;
	tdescptr->threadcontext.uc_link = 0;
	tdescptr->threadcontext.uc_stack.ss_flags = 0;
	tdescptr->threadfunc = fn;

	makecontext(&(tdescptr->threadcontext), fn, 0);

    struct queue_entry *node = queue_new_node(tdescptr);
    queue_insert_tail(&q, node);

	numthreads += 1;

	return true;
}

void sut_yield()
{
    struct queue_entry *elem = queue_pop_head(&q);
    queue_insert_tail(&q, elem);
	swapcontext(&(elem->thread->threadcontext), &c_exec_context);
}

void sut_exit()
{
    struct queue_entry *elem = queue_pop_head(&q);
	numthreadscancelled += 1;
	setcontext(&c_exec_context);
}

void sut_open(char *dest, int port);
void sut_write(char *buf, int size);
void sut_close();
char *sut_read();

void sut_shutdown()
{
	pthread_join(tid_c_exec,NULL);
}

