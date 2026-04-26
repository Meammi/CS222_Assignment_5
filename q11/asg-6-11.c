#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t mutex;
int n;

void *foo(void *arg)
{
    for (int i = 0; i < n; i++)
    {
        printf("foo lock %d\n", i + 1);
        pthread_mutex_lock(&mutex);
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
        printf("bar lock %d\n", count);
        pthread_mutex_lock(&mutex);
    }

    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc != 2) return 1;

    n = atoi(argv[1]);

    pthread_t t1, t2;

    pthread_mutex_init(&mutex, NULL);

    pthread_create(&t1, NULL, foo, NULL);
    pthread_create(&t2, NULL, bar, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    pthread_mutex_destroy(&mutex);

    return 0;
}