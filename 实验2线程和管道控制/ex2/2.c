/* 
* 
* 
* 
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include<math.h>
void task1(int *);    //线程 1 执行函数原型
void task2(int *);    //线程 2 执行函数原型
void task3(int *);
int pipe1[2],pipe2[2];   //存放第两个无名管道标号
pthread_t thrd1,thrd2,thrd3;   //存放第两个线程标识
int main(int argc,char *arg[])
{
    int ret;
    int x,y;
//使用 pipe()系统调用建立两个无名管道。建立不成功程序退出，执行终止
    if(pipe(pipe1) < 0){
        perror("pipe1 not create");
        exit(EXIT_FAILURE);
    }
    if(pipe(pipe2) < 0)
    {
        perror("pipe2 not create");
        exit(EXIT_FAILURE);
    }
//使用 pthread_create 系统调用建立两个线程。建立不成功程序退出，执行终止
    x = 1 ;
    y = 2 ;
    printf("请输入xy：");
    scanf("%d%d",&x,&y);
    ret = pthread_create(&thrd1,NULL,(void *) task1,(void *) &x);
    if(ret){
        perror("pthread_create: task1");
        exit(EXIT_FAILURE);
    }
    
    ret = pthread_create(&thrd2,NULL,(void *) task2,(void *) &y);
    if(ret){
        perror("pthread_create: task2");
        exit(EXIT_FAILURE);
    }
    ret = pthread_create(&thrd3,NULL,(void *) task3,(void *) &x);
    if(ret){
        perror("pthread_create: task3");
        exit(EXIT_FAILURE);
    }
//挂起当前线程，等待线程thrd2结束，并回收其资源
    pthread_join(thrd2,NULL);
//挂起当前线程，等待线程thrd1结束，并回收其资源
    pthread_join(thrd1,NULL);
    pthread_join(thrd3,NULL);
    exit(EXIT_SUCCESS);
}
//线程 1 执行函数
void task1(int *x)
{
    int fx = 1;
    for (int i = 1; i <= *x; i++) fx *= i;
    printf("f(x)=%d\n", fx);
    write(pipe1[1], &fx, sizeof(int));
    close(pipe1[1]);
    
}

//线程 2 执行函数
void task2(int * y)
{
    double q1 = (1 + sqrt(5)) / 2, q2 = (1 - sqrt(5)) / 2;
    int fy = (pow(q1, *y) - pow(q2, *y)) / sqrt(5); 
    printf("f(y)=%d\n", fy);
    write(pipe2[1], &fy, sizeof(int));
    close(pipe2[1]);
}
void task3(int *x)
{
    int fx, fy;
    read(pipe1[0], &fx, sizeof(int));
    read(pipe2[0], &fy, sizeof(int));
    printf("f(x+y)=%d\n", fx + fy);
    close(pipe1[0]);
    close(pipe2[0]);
}

