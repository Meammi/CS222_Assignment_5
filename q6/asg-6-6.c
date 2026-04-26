#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>

#define SIZE 10

int q[SIZE];
int front = 0, rear = 0, countQ = 0;

int np, nc, nx, ny;

pthread_mutex_t mutex;
pthread_cond_t not_full, not_empty;
pthread_barrier_t barrier;

struct timeval starttime, endtime;

void en_Q(int data)
{
    q[rear] = data;
    rear = (rear + 1) % SIZE;
    countQ++;
}

int de_Q()
{
    int x = q[front];
    front = (front + 1) % SIZE;
    countQ--;
    return x;
}

void *producer(void *arg)
{
    //Convert to int
    int rank = *(int*)arg;

    //เปิด Barrier
    pthread_barrier_wait(&barrier);

    //Producer ตัวแรกจะเป็นคนจับเวลา
    if(rank == 1)
        gettimeofday(&starttime, NULL);
    // Loop
    for(int i=0;i<nx;i++)
    {
        sleep(1);
        //เปิด mutex
        pthread_mutex_lock(&mutex);
        //ถ้า queue full รอจนกว่า consumer จะ removes item
        while(countQ == SIZE)
            pthread_cond_wait(&not_full, &mutex);

        en_Q(rank);
        printf("produce %d\n", rank);
        //ส่ง cond ว่าตอนนี้ queue ไม่ว่าง
        pthread_cond_signal(&not_empty);
        pthread_mutex_unlock(&mutex);
        //ปิด mutex
    }
    //ปิด Berrier
    pthread_barrier_wait(&barrier);

    //ให้ producer ที่เป็น rank 1 คำนวณ finish time
    if(rank == 1)
    {
        gettimeofday(&endtime, NULL);

        double sec =
        (endtime.tv_sec - starttime.tv_sec) +
        (endtime.tv_usec - starttime.tv_usec)/1000000.0;

        printf("Total Time = %.6f sec\n", sec);
    }

    return NULL;
}

void *consumer(void *arg)
{
    int rank = *(int*)arg;

    pthread_barrier_wait(&barrier);

    for(int i=0;i<ny;i++)
    {
        pthread_mutex_lock(&mutex);
        //ถ้า queue ว่างให้ watiting จนกว่า Producer จะ inserts data
        while(countQ == 0)
            pthread_cond_wait(&not_empty, &mutex);
        //remove item from queue
        int data = de_Q();

        printf("consume %d got %d\n", rank, data);
        //ส่งสัญญาณว่าง Queue ตอนนี้ไม่เต็ม
        pthread_cond_signal(&not_full);
        pthread_mutex_unlock(&mutex);
    }

    pthread_barrier_wait(&barrier);

    return NULL;
}

int main(int argc, char *argv[])
{
    //เช็คจน. Argument
    if(argc != 4) return 1;

    //Assign
    np = atoi(argv[1]); //จน.Thread producer
    nc = atoi(argv[2]); //จน.Thread consumer
    nx = atoi(argv[3]); //จน.รอบเพิ่มลบข้อมูล

    ny = (np * nx) / nc;

    //Thread identifier
    pthread_t p[np], c[nc];
    //Rank array
    int pr[np], cr[nc];

    //Synchronization object
    pthread_mutex_init(&mutex,NULL);
    pthread_cond_init(&not_full,NULL);
    pthread_cond_init(&not_empty,NULL);

    //barrier
    pthread_barrier_init(&barrier,NULL,np+nc);

    //Loop สร้าง Thread producer
    for(int i=0;i<np;i++){
        pr[i]=i+1;
        pthread_create(&p[i],NULL,producer,&pr[i]);
    }

    //Loop สร้าง Thread consumer
    for(int i=0;i<nc;i++){
        cr[i]=i+1;
        pthread_create(&c[i],NULL,consumer,&cr[i]);
    }

    //Loop รอ Thread ทำงานเสร็จ
    for(int i=0;i<np;i++) pthread_join(p[i],NULL);
    for(int i=0;i<nc;i++) pthread_join(c[i],NULL);

    return 0;
}
