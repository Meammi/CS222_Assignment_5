// asg6-15-good.c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

int n, k;
sem_t *forks;

int left(int p)  { return p; }
int right(int p) { return (p + 1) % n; }

void think_fn() { sleep(1); }
void eat_fn()   { sleep(2); }

void get_fork(int p)
{
    if(p == n-1)   // last philosopher reverses order
    {
        sem_wait(&forks[right(p)]);
        sem_wait(&forks[left(p)]);
    }
    else
    {
        sem_wait(&forks[left(p)]);
        sem_wait(&forks[right(p)]);
    }
}

void put_fork(int p)
{
    sem_post(&forks[left(p)]);
    sem_post(&forks[right(p)]);
}

void *philosopher(void *arg)
{
    int rank = *(int*)arg;

    for(int i=0;i<k;i++)
    {
        think_fn();

        printf("--> Rank %d WAITS to get fork\n", rank);
        get_fork(rank-1);
        printf("--> Rank %d GOT fork\n", rank);

        eat_fn();

        printf("<-- Rank %d PUT fork\n", rank);
        put_fork(rank-1);
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    if(argc != 3) return 1;

    n = atoi(argv[1]);
    k = atoi(argv[2]);

    pthread_t th[n];
    int id[n];

    forks = malloc(sizeof(sem_t)*n);

    for(int i=0;i<n;i++)
        sem_init(&forks[i],0,1);

    for(int i=0;i<n;i++){
        id[i]=i+1;
        pthread_create(&th[i],NULL,philosopher,&id[i]);
    }

    for(int i=0;i<n;i++)
        pthread_join(th[i],NULL);

    return 0;
}