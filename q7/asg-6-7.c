#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int count = 0;
int n;

pthread_mutex_t mutex;
pthread_cond_t cond;

void *foo(void *arg)
{
    long rank = (long)arg;

    //ล็อค mutex
    pthread_mutex_lock(&mutex);

    count++;
    printf("Thread %ld arrived, count = %d\n", rank, count);

    // เช็คว่า Count < n ไหมถ้าเกิดว่าใช่ก็ให้ cond รอไปก่อนเพราะว่าเราต้องการจำลองการทำงานของ barier ถ้าเกิดว่า count == n แล้วหมายความว่าทุกตัวมาถึงแล้ว
    if (count < n)
    {
        while (count < n)
        {
            pthread_cond_wait(&cond, &mutex); // Unlock mutex -> Sleep -> Wait for signal -> Lock mutex
        }

        printf("Thread %ld wakes up, count = %d\n", rank, count);
    }
    else
    {
        // wake ทุกตัวที่ waiting
        pthread_cond_broadcast(&cond);
    }

    //ปล็ด mutex
    pthread_mutex_unlock(&mutex);

    printf("Thread %ld Done\n", rank);

    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc != 2){return 1;}

    n = atoi(argv[1]);

    //thread identifier
    pthread_t threads[n];

    //Synchronization object
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    //Loop สร้าง Thread
    for (long i = 1; i <= n; i++)
    {
        pthread_create(&threads[i - 1], NULL, foo, (void *)i);
    }

    //Loop รอ Thread ทำงานเสร็จ
    for (int i = 0; i < n; i++)
    {
        pthread_join(threads[i], NULL);
    }

    //Clean object
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    return 0;
}