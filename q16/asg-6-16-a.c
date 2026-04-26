// asg6-16a.c

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

typedef struct {
    int value;
    pthread_mutex_t lock;
    pthread_cond_t cond;
} zem_t;

/* init */
void zem_init(zem_t *z, int value)
{
    z->value = value;
    pthread_mutex_init(&z->lock, NULL);
    pthread_cond_init(&z->cond, NULL);
}

/* wait */
void zem_wait(zem_t *z)
{
    pthread_mutex_lock(&z->lock);

    while (z->value <= 0)
        pthread_cond_wait(&z->cond, &z->lock);

    z->value--;

    pthread_mutex_unlock(&z->lock);
}

/* post */
void zem_post(zem_t *z)
{
    pthread_mutex_lock(&z->lock);

    z->value++;
    pthread_cond_signal(&z->cond);

    pthread_mutex_unlock(&z->lock);
}

zem_t sem;

void *foo(void *arg)
{
    printf("foo waiting...\n");
    zem_wait(&sem);

    printf("foo entered critical section\n");
    sleep(2);

    printf("foo leaving\n");
    zem_post(&sem);

    return NULL;
}

int main()
{
    pthread_t t1, t2;

    zem_init(&sem, 1);

    pthread_create(&t1, NULL, foo, NULL);
    pthread_create(&t2, NULL, foo, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    return 0;
}