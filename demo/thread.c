#include "m_stdio.h"
#include <pthread.h>


/////////////////////////////////////////////////////
//                    main_1                      //
////////////////////////////////////////////////////
static unsigned char flag_1 = 1;
static void * thr_fn_1(void * arg)
{
    while (flag_1)
    {
        printf("thread sleep 10 second\n");
        sleep(10);
    }
    printf("thread exit\n");
}

/*
*
*输出:
* thread sleep 10 second
* >q    // 输入
* Now terminate the thread!
* Wait for thread to exit   // 等待
* thread exit
* Bye
*
*/
static int main_1()
{
    pthread_t thread_id;
    if (0 != pthread_create(&thread_id, NULL, thr_fn_1, NULL))
    {
        printf("error when create pthread,%d\n", errno);
        return 1;
    }

    char c ;
    while ((c = getchar()) != 'q');

    // 输入 'q'
    printf("Now terminate the thread!\n");
    flag_1 = 0;
    printf("Wait for thread to exit\n");

    // 阻塞至线程(thread_id)结束  可能要等待10S 至线程从sleep中醒来
    pthread_join(thread_id, NULL);
    printf("Bye\n");
    return 0;
}

/////////////////////////////////////////////////////
//                    main_2                      //
////////////////////////////////////////////////////
static pthread_cond_t cond;
static pthread_mutex_t mutex;
static unsigned char flag_2 = 1;
static unsigned char flag_2_1 = 1;
static void * thr_fn_2(void * arg)
{
    struct timeval now;
    struct timespec outtime;
    int thread_id = pthread_self();
    printf("%s id = %ld(0x%x)\n",arg,thread_id,&thread_id);
    pthread_mutex_lock(&mutex);
    while (flag_2)
    {
        printf("thread(%ld) sleep now\n",thread_id);
        gettimeofday(&now, NULL);
        outtime.tv_sec = now.tv_sec + 10;
        outtime.tv_nsec = now.tv_usec * 1000;
        // 暂停线程
        // pthread_cond_timedwait 会先解锁mutex,再加锁mutex
        pthread_cond_timedwait(&cond, &mutex, &outtime);
    }
    pthread_mutex_unlock(&mutex);
    printf("thread(%ld) exit\n",thread_id);
}

/*
*
*输出:
* thread sleep 10 second
* >q    // 输入
* Now terminate the thread!
* Wait for thread to exit   // 不等待
* thread exit
* Bye
*
*/
int main_2()
{
    pthread_t thread,thread_1;
    char c ;
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
    if (0 != pthread_create(&thread, NULL, thr_fn_2, "thread_1"))
    {
        printf("error when create pthread1,%d\n", errno);
        return 1;
    }

    if (0 != pthread_create(&thread_1, NULL, thr_fn_2, "thread_2"))
    {
        printf("error when create pthread2,%d\n", errno);
        return 1;
    }

    while ((c = getchar()) != 'q');
    printf("Now terminate the thread!\n");
    flag_2 = 0;
    pthread_mutex_lock(&mutex);
    pthread_cond_signal(&cond); //激活等待的一个线程
    //pthread_cond_broadcast(&cond); // 激活等待的所有线程
    pthread_mutex_unlock(&mutex);
    printf("Wait for thread to exit\n");
    pthread_join(thread_1, NULL); // 不会阻塞
    printf("Bye\n");
    return 0;
}




int main(int argc,char *argv)
{
    //return main_1();
    return main_2();
}