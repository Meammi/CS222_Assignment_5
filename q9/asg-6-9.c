#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

sem_t seats;
int et;

void *customer(void *arg)
{
    int rank = *(int *)arg;

    printf("--> rank id %d is waiting for a seat\n", rank);
    //ถ้ามี seat, customer จะนั่ง ถ้าไม่มี, thread blocks
    sem_wait(&seats);

    printf("--> rank id %d got a seat\n", rank);

    sleep(et);

    printf("<-- rank id %d is leaving\n", rank);

    //return seat คืน
    sem_post(&seats);

    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc != 4){return 1;}

    int ns = atoi(argv[1]);
    int nc = atoi(argv[2]);
    et = atoi(argv[3]);

    pthread_t threads[nc];
    int rank[nc];

    //semaphore
    sem_init(&seats, 0, ns);

    for (int i = 0; i < nc; i++)
    {
        rank[i] = i + 1;
        pthread_create(&threads[i], NULL, customer, &rank[i]);
    }

    for (int i = 0; i < nc; i++)
    {
        pthread_join(threads[i], NULL);
    }

    sem_destroy(&seats);

    return 0;
}