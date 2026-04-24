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
    // สร้าง Thread identifiers สองตัว
    pthread_t t1, t2;

    // สร้าง mutex กับ cond (synchronization object)
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    // สร้าง Thread & given function ให้ Thread แล้วเริ่ม (spawn & start)
    pthread_create(&t1, NULL, bar, NULL);
    pthread_create(&t2, NULL, foo, NULL);

    // Wait Thread ทั้งสองตัวจนกว่าจะเสร็จ
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    // Destroy synchronization objects
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    return 0;
}