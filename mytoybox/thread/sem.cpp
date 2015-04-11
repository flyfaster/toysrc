#include <unistd.h>     /* Symbolic Constants */
#include <sys/types.h>  /* Primitive System Data Types */
#include <errno.h>      /* Errors */
#include <stdio.h>      /* Input/Output */
#include <stdlib.h>     /* General Utilities */
#include <pthread.h>    /* POSIX Threads */
#include <string.h>     /* String handling */
#include <semaphore.h>  /* Semaphore */
#include <sys/time.h>
/* global vars */
/* semaphores are declared global so they can be accessed
   in main() and in thread routine,
   here, the semaphore is used as a mutex */
sem_t mutex;
int counter; /* shared variable */

void* handler ( void *ptr )
{
	pthread_t x = pthread_self();
    printf("Thread %u: Waiting to enter critical region...\n", x);
    sem_wait(&mutex);       /* down semaphore */
    printf("Thread %u: Now in critical region...\n", x);
    printf("Thread %u: Counter Value: %d\n", x, counter);
    printf("Thread %u: Incrementing Counter...\n", x);
    counter++;
    usleep(1000);
    printf("Thread %u: New Counter Value: %d\n", x, counter);
    printf("Thread %u: Exiting critical region...\n", x);
    sem_post(&mutex);
    pthread_exit(0);
}
pthread_mutex_t count_mutex;
pthread_mutexattr_t Attr;

void* handler2 ( void *ptr )
{
	pthread_t x = pthread_self();
    printf("Thread %u: Waiting to enter critical region...\n", x);
    pthread_mutex_lock(&count_mutex);
    printf("Thread %u: Now in critical region...\n", x);
    printf("Thread %u: Counter Value: %d\n", x, counter);
    printf("Thread %u: Incrementing Counter...\n", x);
    counter++;
    usleep(1000);
    printf("Thread %u: New Counter Value: %d\n", x, counter);
    printf("Thread %u: Exiting critical region...\n", x);
    pthread_mutex_unlock(&count_mutex);
    pthread_exit(0);
}

int main()
{
	printf("try semaphore and mutex\n");
	pthread_mutexattr_init(&Attr);
	pthread_mutexattr_settype(&Attr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&count_mutex, &Attr);

    int i[2] ={0};
    pthread_t thread_a;
    pthread_t thread_b;
//    if ((mutex = sem_open("mysemaphore", O_CREAT, 0644, 1)) == SEM_FAILED) {
//      perror("semaphore initilization");
//      exit(1);
//    }
    sem_init(&mutex, 0, 1);      /* initialize mutex to 1 - binary semaphore */
                                 /* second param = 0 - semaphore is local */

    /* Note: you can check if thread has been successfully created by checking return value of
       pthread_create */
    pthread_create (&thread_a, NULL, &handler, (void *) &i[0]);
    pthread_create (&thread_b, NULL, &handler, (void *) &i[1]);

    pthread_join(thread_a, NULL);
    pthread_join(thread_b, NULL);

    sem_destroy(&mutex); /* destroy semaphore */
    handler2(0); // just check syntax of mutex.
    pthread_mutex_destroy(&count_mutex);
    return (0);
}


