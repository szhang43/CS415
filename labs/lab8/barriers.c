/*
This file serves to show how to use multithreading while
avoiding deadlocks using a single worker thread example and
a multi worker thread example

Author: Alex Summers (The Goat)
*/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <syscall.h>

/*
pthread_cond_wait() -> halts execution until
1. is signaled
2. the lock is available to take

this function halts until it is signaled. when it is
it tries to take the lock if avaible halts otherwise.
So it waits for two seperate things until it resumes exection.
*/



#define NUM_WORKERS 5

void *worker_func_multi_thread_example(void *arg);

void *banker_func_multi_thread_example(void *arg);

pthread_mutex_t worker_done_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t bank_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t bank_cond = PTHREAD_COND_INITIALIZER; // to signal bank thread
pthread_cond_t worker_cond = PTHREAD_COND_INITIALIZER;

pthread_barrier_t barrier;// barrier sycnro object

int workers_done = 0;


int main(int argc, char *argv[])
{
    pthread_t bank_thread, worker_thread;
    pthread_t worker_threads[NUM_WORKERS];

    int ret = pthread_barrier_init(&barrier, NULL, 2);

    void* (*banker_func)(void*) = &banker_func_multi_thread_example;
    void* (*worker_func)(void*) = &worker_func_multi_thread_example;


    printf("Taking bank lock and creating banker\n");
    pthread_mutex_lock(&bank_lock);
    pthread_create(&bank_thread, NULL, banker_func, NULL);


    // wait for bank thread to cond wait
    pthread_cond_wait(&bank_cond, &bank_lock);
    printf("Main thread wakes up from banker signal and freeing of bank_lock\n");
    pthread_mutex_unlock(&bank_lock);

    printf("Main thread creating workers\n");
    for(int i = 0; i < NUM_WORKERS; i++){
        pthread_create(&worker_threads[i], NULL, worker_func, NULL);
    }

    // like waitpid we are waiting for our threads to be done
    // NOTE: If master thread exits before child threads are done
    // the whole process is terminated

    for(int i = 0; i < NUM_WORKERS; i++){
        pthread_join(worker_threads[i], NULL);
    }

    printf("Workers Exited and Banker signaled\n");

    pthread_cond_signal(&bank_cond); 

    pthread_join(bank_thread, NULL);
}

void *worker_func_multi_thread_example(void *arg){
    
    printf("WORKER DOING WORK\n");

    pthread_mutex_lock(&worker_done_lock);

    workers_done++;
    printf("worker %d in critical section\n", workers_done);

    if(workers_done == NUM_WORKERS){

        pthread_mutex_lock(&bank_lock);

        pthread_cond_signal(&bank_cond);

        printf("Last worker #%d signals bank and halts waiting on bank_lock\n", workers_done);

        workers_done = 0;

        pthread_cond_wait(&worker_cond, &bank_lock);

        printf("Last worker wakes and frees other workers by unlocking the workers_done_lock\n");

        pthread_mutex_unlock(&bank_lock);

        // rest of workers wake up
        pthread_mutex_unlock(&worker_done_lock);
    }
    else{
        printf("%d worker(s) are halted waiting worker lock\n", workers_done);

        pthread_cond_wait(&worker_cond, &worker_done_lock);
    }

    printf("%d worker(s) is done\n", ++workers_done);
    // release lock immediately so other workers can wake up
    pthread_mutex_unlock(&worker_done_lock);
}

void *banker_func_multi_thread_example(void *arg){
    pthread_mutex_lock(&bank_lock);
    pthread_cond_signal(&bank_cond); // signal main thread

    // release main thread from halt by releasing lock
    printf("Bank signals main thread to wake up then release lock for main thread to wake up\n");
    pthread_cond_wait(&bank_cond, &bank_lock);

    printf("Banker wakes up from last worker signal and does work does work\n");

    pthread_cond_broadcast(&worker_cond);

    printf("Bank broadcasts to workers and halts releasing bank_lock\n");

    pthread_cond_wait(&bank_cond, &bank_lock);

    printf("bank woken up signaled by main thread and now is done\n");
}

#endif