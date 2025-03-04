/* 
 * Filename: customer.c
 * Function: 建立并模拟顾客进程
 */

#include "ipc.h"
key_t buff_key;
int buff_num;
char *buff_ptr;
int shm_flg;

int quest_flg;
key_t quest_key;
int quest_id;

int respond_flg;
key_t respond_key;
int respond_id;

// room请求sofa相关变量
int sofa_quest_flg;
key_t sofa_quest_key;
int sofa_quest_id;
// sofa请求barber相关变量
int barber_quest_flg;
key_t barber_quest_key;
int barber_quest_id;
// barber响应sofa相关变量
int barber_respond_flg;
key_t barber_respond_key;
int barber_respond_id;
// customer请求account相关变量
int account_quest_flg;
key_t account_quest_key;
int account_quest_id;
// barber响应account相关变量
int account_respond_flg;
key_t account_respond_key;
int account_respond_id;
// 账本相关信号量
key_t account_key;
int account_sem;
// 信号量赋值/flag
int sem_flg;
int sem_val;

int get_ipc_id(char *proc_file, key_t key);
char *set_shm(key_t shm_key, int shm_num, int shm_flag);
int set_msq(key_t msq_key, int msq_flag);
int set_sem(key_t sem_key, int sem_val, int sem_flag);
int sem_wait(int sem_id);
int sem_signal(int sem_id);
int main(int argc, char *argv[]) {
    int rate;
    if(argv[1] != NULL)
        rate = atoi(argv[1]);
    else 
        rate = 5;   
     
    Msg_buf msg_arg;
    // room队列, 可容纳13人
    sofa_quest_flg = IPC_CREAT | 0644;
    sofa_quest_key = 111;
    sofa_quest_id = set_msq(sofa_quest_key, sofa_quest_flg);
    //barber队列, 可容纳3人
    barber_quest_flg = IPC_CREAT | 0644;
    barber_quest_key = 211;    
    barber_quest_id = set_msq(barber_quest_key, barber_quest_flg);    
    barber_respond_flg = IPC_CREAT | 0644;
    barber_respond_key = 212;    
    barber_respond_id = set_msq(barber_respond_key, barber_respond_flg);
    //结帐队列, 可容纳3个
    account_quest_flg = IPC_CREAT | 0644;
    account_quest_key = 213;
    account_quest_id = set_msq(account_quest_key, account_quest_flg);
    account_respond_flg = IPC_CREAT | 0644;
    account_respond_key = 214;    
    account_respond_id = set_msq(account_respond_key, account_respond_flg);  
  
    account_key = 312;    
    sem_flg = IPC_CREAT | 0644;
    sem_val = 1;    
    account_sem = set_sem(account_key, sem_val, sem_flg);

    int sofa = 4;   
    int room = 13;
    int chair = 3; 
    int id=0;

    while (1) {
        sleep(rate);
        id++;
        msg_arg.mid = id;
        msg_arg.mtype = id + 1;
        if (sofa > 0) {//沙发有空位
            if (room == 13) {//等候室没人，直接进入沙发
                sofa--;
            } else if (room > 0) {//进入等候室等候
                room--;
                msg_arg.mtype = SOFAQUEST;//请求沙发
                msgsnd(sofa_quest_id, &msg_arg, sizeof(msg_arg), 0);
            } else {//等候室满
                printf("理发店满员, 顾客 %d 离开\n", id);
            }
        }
        else if (room > 0){//沙发满了,等待室有空位
            room--;
            msg_arg.mtype = SOFAQUEST;//请求沙发
            msgsnd(sofa_quest_id, &msg_arg, sizeof(msg_arg), 0);
        } else//等候室满
            printf("理发店满员, 顾客 %d 离开\n", id);

        if (sofa > 0) {//沙发有空位，处理沙发请求
            if (msgrcv(sofa_quest_id, &msg_arg, sizeof(msg_arg), SOFAQUEST, IPC_NOWAIT) >= 0) {
                sofa--;
                room++;
            }
        }

        if (chair > 0) {//理发椅有空位，发出请求
            printf("顾客 %d 请求理发\n", msg_arg.mid);
            sofa++;
            chair--;
            msg_arg.mtype = HAIRQUEST;
            msgsnd(barber_quest_id, &msg_arg, sizeof(msg_arg), 0);
        }
        
        if (msgrcv(barber_respond_id, &msg_arg, sizeof(msg_arg), CUT_FINISHED, IPC_NOWAIT) >= 0) {//理发完成，发出结帐请求
            chair++;       
            msg_arg.mtype = ACCOUNTQUEST;         
            msgsnd(account_quest_id, &msg_arg, sizeof(msg_arg), 0);
        }
        
        if (msgrcv(account_respond_id, &msg_arg, sizeof(msg_arg), ACC_FINISHED, IPC_NOWAIT) >= 0) {//结帐完成
            //printf("顾客 %d 结帐\n", msg_arg.mid);    
            
        }
        
    }

    return EXIT_SUCCESS;
}
