#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int count = 0;

pthread_mutex_t mutex;
pthread_cond_t cond;

// Thread 1
void *foo(void *arg){
    // Lock mutex ก่อนเข้า Critical Section
    pthread_mutex_lock(&mutex);

    count++;
    printf("foo(): count = %d\n", count);
    // ส่ง Signal เพื่อ Wake Thread ที่กำลัง Waiting อยู่
    pthread_cond_signal(&cond);
    // Unlock mutex
    pthread_mutex_unlock(&mutex);
    // End Thread
    pthread_exit(NULL);
}

// Thread 2
void *bar(void *arg){
    // Lock mutex
    pthread_mutex_lock(&mutex);
    // ถ้า Count <= 0 เราจะ Unlock mutex รอ(sleep)จนกว่าจะได้ Signal แล้วก็ทำการ Lock mutex แล้วทำขั้นต่อไป
    while (count <= 0){
       pthread_cond_wait(&cond, &mutex);
    }

    count--;
    printf("bar(): count = %d\n", count);
    // Unlock mutex
    pthread_mutex_unlock(&mutex);
    // End Thread
    pthread_exit(NULL);
}

int main(){
    // สร้าง Thread identifiers array
    pthread_t threads[8];

    // สร้าง mutex กับ cond (synchronization object)
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    for (int i=0; i<4; i++)
        pthread_create(&threads[i], NULL, bar, NULL);
    
    for (int i=4; i<8; i++)
        pthread_create(&threads[i], NULL, foo, NULL);
    
    for (int i=0; i<8; i++)
        pthread_join(threads[i], NULL);
    

    // Destroy synchronization objects
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    return 0;
}