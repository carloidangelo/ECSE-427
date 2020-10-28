#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <time.h>
#include "queue.h" 
#include "thread.h"   
#include "a1_lib.h"                                                            
#include "sut.h"
#define BUFSIZE 128

struct queue q_task; // task ready queue
struct queue_entry *i_exec_node; // pending I/O operation
// don't actually need a queue because  we assume only one 
// read or write call could be pending at any given time

pthread_t tid_c_exec; // C-EXEC thread
ucontext_t c_exec_context; // C-EXEC thread context is saved here
pthread_t tid_i_exec; // I-EXEC thread
ucontext_t i_exec_context; // I-EXEC thread context is saved here

pthread_mutex_t lock; // mutex lock

threaddesc threadarr[MAX_THREADS];
int numthreads, numthreadscancelled;

int sockfd; // socket file descriptor given after sut_open()
char read_msg[BUFSIZE]; // holds message received from sut_read()

void *c_exec()
{
	struct queue_entry *elem;
	struct timespec tim, tim2;
	while(1){
		pthread_mutex_lock(&lock);
		elem = queue_peek_front(&q_task);
		pthread_mutex_unlock(&lock);
		if (numthreads > 0 && numthreads-numthreadscancelled == 0){
			pthread_exit(0);
		}else if (elem == NULL){
   			tim.tv_sec = 0;
   			tim.tv_nsec = 100 * 1000;
			nanosleep(&tim,&tim2);
		}else{
			swapcontext(&c_exec_context, &(elem->thread->threadcontext));
		}
	}
}

void *i_exec()
{
	struct timespec tim, tim2;
	while(1){
		if (numthreads > 0 && numthreads-numthreadscancelled == 0){
			pthread_exit(0);
		}else if (i_exec_node == NULL){
   			tim.tv_sec = 0;
   			tim.tv_nsec = 1;
			nanosleep(&tim,&tim2);
		}else{
			swapcontext(&i_exec_context, &(i_exec_node->thread->threadcontext));
			pthread_mutex_lock(&lock);
			queue_insert_tail(&q_task, i_exec_node);
			pthread_mutex_unlock(&lock);
			i_exec_node = NULL;
		}
	}
}

void sut_init()
{
    q_task = queue_create();
    queue_init(&q_task);
	numthreads = 0;
	numthreadscancelled = 0;
	pthread_mutex_init(&lock, NULL);
	pthread_create(&tid_c_exec, NULL, c_exec, NULL);
	pthread_create(&tid_i_exec, NULL, i_exec, NULL);
}

bool sut_create(sut_task_f fn)
{
	threaddesc *tdescptr;

	if (numthreads >= MAX_THREADS) 
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

	pthread_mutex_lock(&lock);
    queue_insert_tail(&q_task, queue_new_node(tdescptr));
	pthread_mutex_unlock(&lock);

	numthreads += 1;

	return true;
}

void sut_yield()
{
	struct queue_entry *elem;
	pthread_mutex_lock(&lock);
    elem = queue_pop_head(&q_task);
    queue_insert_tail(&q_task, elem);
	pthread_mutex_unlock(&lock);
	swapcontext(&(elem->thread->threadcontext), &c_exec_context);
}

void sut_exit()
{
	pthread_mutex_lock(&lock);
    queue_pop_head(&q_task);
	pthread_mutex_unlock(&lock);
	numthreadscancelled += 1;
	setcontext(&c_exec_context);
}

void sut_open(char *dest, int port)
{
	struct queue_entry *elem;
	pthread_mutex_lock(&lock);
	elem = queue_pop_head(&q_task);
	pthread_mutex_unlock(&lock);
	i_exec_node = elem;
	swapcontext(&(elem->thread->threadcontext), &c_exec_context);
	connect_to_server(dest, port, &sockfd);
	swapcontext(&(elem->thread->threadcontext), &i_exec_context);
}

void sut_write(char *buf, int size)
{
	struct queue_entry *elem;
	pthread_mutex_lock(&lock);
	elem = queue_pop_head(&q_task);
	pthread_mutex_unlock(&lock);
	i_exec_node = elem;
	swapcontext(&(elem->thread->threadcontext), &c_exec_context);
	send_message(sockfd, buf, size);
	swapcontext(&(elem->thread->threadcontext), &i_exec_context);
}

void sut_close()
{
	struct queue_entry *elem;
	pthread_mutex_lock(&lock);
	elem = queue_pop_head(&q_task);
	pthread_mutex_unlock(&lock);
	i_exec_node = elem;
	swapcontext(&(elem->thread->threadcontext), &c_exec_context);
	close(sockfd);
	swapcontext(&(elem->thread->threadcontext), &i_exec_context);
}

char *sut_read()
{
	struct queue_entry *elem;
	pthread_mutex_lock(&lock);
	elem= queue_pop_head(&q_task);
	pthread_mutex_unlock(&lock);
	i_exec_node = elem;
	swapcontext(&(elem->thread->threadcontext), &c_exec_context);
	memset(read_msg, 0, BUFSIZE);
	recv_message(sockfd, read_msg, BUFSIZE);
	swapcontext(&(elem->thread->threadcontext), &i_exec_context);
	return read_msg;
}

void sut_shutdown()
{
	pthread_join(tid_c_exec,NULL);
	pthread_join(tid_i_exec,NULL);
	pthread_mutex_destroy(&lock);
}

