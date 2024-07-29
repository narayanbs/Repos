/*
 Barriers

 C library functions – <pthread.h>

 int pthread_barrier_init (pthread_barrier_t *barrier, const pthread_barrierattr_t *attr, unsigned count);
 Initialize a synchronization barrier with the specified attributes.

 int pthread_barrier_destroy (pthread_barrier_t *barrier);
 Delete a synchronization barrier.

 int pthread_barrier_wait (pthread_barrier_t *barrier);
 Make a thread wait until enough have reached the barrier.
*/
#include <pthread.h>
#include <stdio.h>
#include <assert.h>

struct args {
    pthread_barrier_t* barrier;
    unsigned int digit;
    double base;
    unsigned long fib;
    unsigned long exp;
};

void* fibonacci(void* _args) {
    struct args* args = (struct args*)_args;
    unsigned long previous = 1;
    unsigned long current = 1;
    unsigned int i;

    for(i = 2; i < args->digit; i++) {
        current += previous;
        previous = current - previous;
    }
    args->fib = current;
    pthread_barrier_wait(args->barrier);
    if(args->fib > args->exp) {
        printf("Fibonacci wins: %lu > %lu\n", args->fib, args->exp);
    }
    pthread_exit(NULL);
}

void* exponential(void* _args) {
    struct args* args = (struct args*)_args;
    double result = 1;
    unsigned int i ;

    for(i = 1; i < args->digit; i++) {
        result *= args->base;
    }
    args->exp = (unsigned long)result;

    pthread_barrier_wait(args->barrier);
    if(args->exp > args->fib) {
        printf("Exponential wins: %lu > %lu\n", args->exp, args->fib);
    }
    pthread_exit(NULL);
}

int main(void) {
    pthread_t threads[2];
    pthread_barrier_t barrier;
    pthread_barrier_init(&barrier, NULL, 2);

    struct args args;
    args.barrier = &barrier;
    args.digit = 30;
    args.base = 1.6;
    args.fib = 0;
    args.exp = 0;

    assert(pthread_create(&threads[0], NULL, fibonacci, &args)==0);
    assert(pthread_create(&threads[1], NULL, exponential, &args)==0);

    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);

    pthread_barrier_destroy(&barrier);
}
