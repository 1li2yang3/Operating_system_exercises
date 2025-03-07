#include "ipc.h"
#include <unistd.h>
// 生产消费者共享缓冲区即其有关的变量
key_t buff_key;
int buff_num;
char *buff_ptr;

// 生产者放产品位置的共享指针
key_t pput_key;
int pput_num;
int *pput_ptr;

// 消费者取产品位置的共享指针
key_t cget_key;
int cget_num;
int *cget_ptr;

// 生产者有关的信号量
key_t prod_key1;
key_t prod_key2;
key_t pmtx_key;
int prod_sem1;
int pmtx_sem;
int prod_sem2;


// 消费者有关的信号量
// key_t cons_key;
key_t cmtx_key;

key_t sm1_key;
key_t sm2_key;
key_t sm3_key;

// int cons_sem;
int cmtx_sem;

key_t sm1_sem;
key_t sm2_sem;
key_t sm3_sem;

int sem_val;
int sem_flg;
int shm_flg;

int type;

int main(int argc,char *argv[])
{
	int rate;
	rate = 3; // 不指定为 3 秒
	// 增加参数指定吸烟者类型
	type = atoi(argv[1]);

	// 共享内存 使用的变量
	buff_key = 101; // 缓冲区任给的键值
	buff_num = 2; // 缓冲区任给的长度
	cget_key = 103; // 消费者取产品指针的键值
	cget_num = 1; // 指针数
	shm_flg = IPC_CREAT | 0644; //共享内存读写权限
	// 获取缓冲区使用的共享内存,buff_ptr 指向缓冲区首地址
	buff_ptr = (char *)set_shm(buff_key,buff_num,shm_flg);
	// 获取消费者取产品指针,cget_ptr 指向索引地址
	cget_ptr = (int *)set_shm(cget_key,cget_num,shm_flg);
	// 信号量使用的变量
	prod_key1 = 201; // 生产者同步信号灯键值
	prod_key2 = 202;
	pmtx_key = 203; // 生产者互斥信号灯键值
	// cons_key = 301; // 消费者同步信号灯键值
	sm1_key = 401;
	sm2_key = 402;
	sm3_key = 403;

	cmtx_key = 302; // 消费者互斥信号灯键值
	sem_flg = IPC_CREAT | 0644; // 信号灯操作权限
	// 生产者同步信号灯初值设为缓冲区最大可用量
	sem_val = buff_num;
	// 获取生产者同步信号灯,引用标识存 prod_sem
	prod_sem1 = set_sem(prod_key1,sem_val,sem_flg);
	prod_sem2 = set_sem(prod_key2,sem_val,sem_flg);
	// 消费者初始无产品可取,同步信号灯初值设为 0
	sem_val = 0;
	// 获取消费者同步信号灯,引用标识存 cons_sem
	sm1_sem = set_sem(sm1_key,sem_val,sem_flg);
	sm2_sem = set_sem(sm2_key,sem_val,sem_flg);
	sm3_sem = set_sem(sm3_key,sem_val,sem_flg);


	sem_val = 1;

	cmtx_sem = set_sem(cmtx_key,sem_val,sem_flg);


	//for (int i = 0; i < buff_num; i++) buff_ptr[i] = 0;
	*cget_ptr=0;
	while (1) {
		if (type == 1) {
			down(sm1_sem);
			while (buff_ptr[*cget_ptr] != 1) {
				*cget_ptr = (*cget_ptr + 1) % buff_num;
			}
			buff_ptr[*cget_ptr] = 0;

			printf("有烟草吸烟者获得: 1(纸、胶水) from Buffer[%d]\n",*cget_ptr);
		}
		else if (type == 2) {
			down(sm2_sem);
			while (buff_ptr[*cget_ptr] != 2) {
				*cget_ptr = (*cget_ptr + 1) % buff_num;
			}
			buff_ptr[*cget_ptr] = 0;
			printf("有纸吸烟者获得: 2(烟草、胶水) from Buffer[%d]\n",*cget_ptr);
		}
		else if (type == 3) {
			down(sm3_sem);
			while (buff_ptr[*cget_ptr] != 3) {
				*cget_ptr = (*cget_ptr + 1) % buff_num;
			}
			buff_ptr[*cget_ptr] = 0;
			printf("有胶水吸烟者获得: 3(烟草、纸) from Buffer[%d]\n",*cget_ptr);
		}
		if(*cget_ptr==0){
			up(prod_sem1);
		}
		if(*cget_ptr==1){
			up(prod_sem2);
		}
		*cget_ptr = (*cget_ptr + 1) % buff_num;
		
	}

	return EXIT_SUCCESS;
}
