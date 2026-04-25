#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

// สร้าง barrier (synchronization object)
pthread_barrier_t barrier;

// Thread function 
void *foo(void *arg)
{
    //convert pointer ที่เข้ามาเป็น integer เช่น &rank[2] -> 3
    int rank = *(int *)arg;

    sleep(rank);

    struct timeval before, after; // ใน document เวลาเราจะใช้ gettimeofday เขาจะมี struct ที่เขา provide ไว้ให้มาให้ใช้คู่กันเเพราะ gettimeofday return สองค่าเราเลยต้องเอา struct มารับ ref:https://man7.org/linux/man-pages/man2/settimeofday.2.html
    // ซึ่ง sturtucre ด้านในจะเป็นลักษณะนี้
    /*  struct timeval {
            time_t      tv_sec;   // seconds
            suseconds_t tv_usec;  // microseconds
        };
    */
    gettimeofday(&before, NULL); // return ค่า seconds, microseconds eg. 14:30:05.523891 เก็บเข้า struct
    
    // Wait at Barrier
    pthread_barrier_wait(&barrier);

    gettimeofday(&after, NULL);// เก็บ after time หลังผ่าน barrier

    double diff =(after.tv_sec - before.tv_sec) + (after.tv_usec - before.tv_usec) / 1000000.0;
    printf("Rank %d waited %.6f seconds\n", rank, diff);

    return NULL;
}
/*Note ความรู้ไม่ใหม่แต่ลืมบ่อย main() เป็น special function name เหตุผลเพราะใน C runtime envrionment เขาจะเก็บ main ไว้ทำให้เวลา execution เขาจะ expect main function ถ้ามี
1._start
2. sets sttack
3. prepares environment variables
4. prepares argc, argv (argc = argument ที่เราเข้ามาจะมีกี่ตัว, argv = argument ที่รับเข้ามามีหน้าตายังไง)
5. init runtime/libc
6. calls main(argc, argv)
7. collects return value
8. calls exit

หมายความว่าสมมติเราอยากทำโปรแกรมที่รับ arg เข้ามาสองตัวเช่น ./square 4 5
ในโปรแกรมเราก็ต้องเช็ค argc != 3 แล้วเราก็ split แยกตัวแปรด้วย wide = atoi(argv[1]), length = atoi(argv[2])
*/
int main(int argc, char *argv[])
{
    if (argc != 2){return 1;}

    int n = atoi(argv[1]);

    // Thread identifier
    pthread_t threads[n];
    int rank[n];
    //barrier คือเราเปิดว่า thread หลังจากนี้จจะต้องรอให้ทุก thread ทำงานเสร็จก่อน
    pthread_barrier_init(&barrier, NULL, n);

    //Loop สร้าง threads
    for (int i = 0; i < n; i++)
    {
        rank[i] = i + 1;
        pthread_create(&threads[i], NULL, foo, &rank[i]);
    }

    //Loop รอให้แต่ละ thread จบ
    for (int i = 0; i < n; i++)
    {
        pthread_join(threads[i], NULL);
    }

    //ปิด barrier
    pthread_barrier_destroy(&barrier);
    return 0;
}