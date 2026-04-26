#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

sem_t sem;

void *foo(void *arg)
{
    printf("this is foo.\n");

    // อนุญาติให้ bar ทำงานต่อ
    sem_post(&sem);

    return NULL;
}

void *bar(void *arg)
{
    //รอจนกว่า foo จะทำงานเสร็จ
    sem_wait(&sem);

    printf("this is bar.\n");

    return NULL;
}

int main()
{
    pthread_t t1, t2;

    //Initialize semaphore
    sem_init(&sem, 0, 0);

    pthread_create(&t1, NULL, foo, NULL);
    pthread_create(&t2, NULL, bar, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    //Clear
    sem_destroy(&sem);

    return 0;
}