/*
* Filename
* copyright
* Function
: ipc.h
: (C) 2006 by zhonghonglie
: 声明 IPC 机制的函数原型和全局变量
*/
#ifndef IPC_H
#define IPC_H
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <unistd.h>
#define BUFFERSIZE 256
#define MAXVAL 100
#define STRSIZE 8
#define CHAIRQUEST 1        // 理发椅请求
#define SOFAQUEST 2         // 沙发请求
#define ROOMQUEST 3         // 等待室请求
#define CUT_FINISHED 4      // 理发结束消息
#define ACC_FINISHED 5      // 结账结束消息
#define HAIRQUEST 5         // 理发请求
#define ACCOUNTQUEST 6      // 结账请求
#define WRITERQUEST 1       // 写标志
#define READERQUEST 2       // 读标志
#define FINISHED 3          // 完成标志
#define SOFA 0
#define ROOM 1
#define CHAIR 2

/* 信号量控制用的共同体 */
typedef union semuns {
    int val;
} Sem_uns;

/* 消息结构体 */
typedef struct msgbuf {
    long mtype;
    int mid;
} Msg_buf;

extern key_t buff_key;
extern int buff_num;
extern char *buff_ptr;
extern int shm_flg;

extern int quest_flg;
extern key_t quest_key;
extern int quest_id;

extern int respond_flg;
extern key_t respond_key;
extern int respond_id;

// room请求sofa相关变量
extern int sofa_quest_flg;
extern key_t sofa_quest_key;
extern int sofa_quest_id;
// sofa请求barber相关变量
extern int barber_quest_flg;
extern key_t barber_quest_key;
extern int barber_quest_id;
// barber响应sofa相关变量
extern int barber_respond_flg;
extern key_t barber_respond_key;
extern int barber_respond_id;
// customer请求account相关变量
extern int account_quest_flg;
extern key_t account_quest_key;
extern int account_quest_id;
// barber响应account相关变量
extern int account_respond_flg;
extern key_t account_respond_key;
extern int account_respond_id;
// 账本相关信号量
extern key_t account_key;
extern int account_sem;
// 信号量赋值/flag
extern int sem_flg;
extern int sem_val;

extern int get_ipc_id(char *proc_file, key_t key);
extern char *set_shm(key_t shm_key, int shm_num, int shm_flag);
extern int set_msq(key_t msq_key, int msq_flag);
extern int set_sem(key_t sem_key, int sem_val, int sem_flag);
extern int sem_wait(int sem_id);
extern int sem_signal(int sem_id);

#endif
