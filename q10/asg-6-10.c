#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

sem_t sem;
int n;

void *foo(void *arg)
{
    for (int i = 0; i < n; i++)
    {
        sem_post(&sem);
        printf("foo: sem_post %d\n", i + 1);
    }

    sleep(10);
    return NULL;
}

void *bar(void *arg)
{
    sleep(3);

    int count = 0;

    while (1)
    {
        count++;
        printf("bar: sem_wait call %d\n", count);
        sem_wait(&sem);
        printf("bar: passed %d\n", count);
    }

    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: ./asg6-10 n\n");
        return 1;
    }

    n = atoi(argv[1]);

    pthread_t t1, t2;

    sem_init(&sem, 0, 1);

    pthread_create(&t1, NULL, foo, NULL);
    pthread_create(&t2, NULL, bar, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    sem_destroy(&sem);

    return 0;
}