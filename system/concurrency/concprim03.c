/*
 Mutual Exclusion with semaphore
 Create the semaphore with count = 1
*/
#include <assert.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>

struct args {
    sem_t* semaphore;
    int value;
};

void* add(void* _args) {
    struct args* args = (struct args*)_args;
    int i;
    for(i = 0; i < 100000; i++) {
        sem_wait(args->semaphore);
        args->value += 10;
        sem_post(args->semaphore);
    }
    pthread_exit(NULL);
}

void* subtract(void* _args) {
    struct args* args = (struct args*)_args;
    int i;
    for(i = 0; i < 100000; i++) {
        sem_wait(args->semaphore);
        args->value -= 10;
        sem_post(args->semaphore);
    }
    pthread_exit(NULL);
}

int main(void) {
    pthread_t threads[2];
    sem_t* sem = sem_open("/OpenCSF_Sema", O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 1);
    assert(sem != SEM_FAILED);
    struct args args = { sem, 0 };

    assert(pthread_create(&threads[0], NULL, add, &args) == 0);
    assert(pthread_create(&threads[1], NULL, subtract, &args) == 0);
    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);

    sem_unlink("/OpenCSF_Sema");
    printf("The end result is %d\n", args.value);
    return 0;
}
