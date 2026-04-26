// asg6-14a.c
// Original code from OSTEP Fig 31.11

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define SIZE 5
#define LOOPS 10

int buffer[SIZE];
int in = 0, out = 0;

sem_t empty, full, mutex;

void put(int x)
{
    buffer[in] = x;
    in = (in + 1) % SIZE;
}

int get()
{
    int x = buffer[out];
    out = (out + 1) % SIZE;
    return x;
}

void *producer(void *arg)
{
    for (int i = 1; i <= LOOPS; i++)
    {
        sem_wait(&mutex);   // WRONG ORDER
        sem_wait(&empty);

        put(i);
        printf("produce %d\n", i);

        sem_post(&full);
        sem_post(&mutex);
    }
    return NULL;
}

void *consumer(void *arg)
{
    for (int i = 1; i <= LOOPS; i++)
    {
        sem_wait(&mutex);   // WRONG ORDER
        sem_wait(&full);

        int x = get();
        printf("consume %d\n", x);

        sem_post(&empty);
        sem_post(&mutex);
    }
    return NULL;
}

int main()
{
    pthread_t p, c;

    sem_init(&empty, 0, SIZE);
    sem_init(&full, 0, 0);
    sem_init(&mutex, 0, 1);

    pthread_create(&p, NULL, producer, NULL);
    pthread_create(&c, NULL, consumer, NULL);

    pthread_join(p, NULL);
    pthread_join(c, NULL);

    return 0;
}