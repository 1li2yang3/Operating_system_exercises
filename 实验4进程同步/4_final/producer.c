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


int type1=0;
int main(int argc, char *argv[])
{
	int rate;
	int to_init = 0;	// 控制初始化缓冲区

	type1 = atoi(argv[1]);//两个供应者 1 2 分别往第0和1个缓存区放材料
	
	rate = 3; // 指定为 3 秒	 

	

		// 共享内存使用的变量
	buff_key = 101; // 缓冲区任给的键值
	buff_num = 2; // 缓冲区任给的长度
	pput_key = 102; // 生产者放产品指针的键值
	pput_num = 1; // 指针数
	shm_flg = IPC_CREAT | 0644; // 共享内存读写权限
	// 获取缓冲区使用的共享内存,buff_ptr 指向缓冲区首地址
	buff_ptr = (char*)set_shm(buff_key, buff_num, shm_flg);
	// 获取生产者放产品位置指针 pput_ptr
	pput_ptr = (int*)set_shm(pput_key, pput_num, shm_flg);
	// 信号量使用的变量
	prod_key1 = 201; // 生产者同步信号灯键值
	prod_key2 = 202;
	pmtx_key = 203; // 生产者互斥信号灯键值
	// cons_key = 301; // 消费者同步信号灯键值
	sm1_key = 401;
	sm2_key = 402;
	sm3_key = 403;

	cmtx_key = 302; // 消费者互斥信号灯键值
	sem_flg = IPC_CREAT | 0644;
	// 生产者同步信号灯初值设为缓冲区最大可用量
	sem_val = buff_num;
	// 获取生产者同步信号灯,引用标识存 prod_sem
	prod_sem1 = set_sem(prod_key1,sem_val,sem_flg);
	prod_sem2 = set_sem(prod_key2,sem_val,sem_flg);
	// 消费者初始无产品可取,同步信号灯初值设为0
	sem_val = 0;
	// 获取消费者同步信号灯,引用标识存 cons_sem
	// cons_sem = set_sem(cons_key,sem_val,sem_flg);
	sm1_sem = set_sem(sm1_key,sem_val,sem_flg);
	sm2_sem = set_sem(sm2_key,sem_val,sem_flg);
	sm3_sem = set_sem(sm3_key,sem_val,sem_flg);
	// 生产者互斥信号灯初值为 1
	sem_val = 1;
	// 获取生产者互斥信号灯,引用标识存 pmtx_sem
	pmtx_sem = set_sem(pmtx_key,sem_val,sem_flg);
	// 循环执行模拟生产者不断放产品



	for (int i = 0; i < buff_num; i++) buff_ptr[i] = 0;
	*pput_ptr =0;
	int ra=-1;
	while (1) {
		if(type1==1){	
			down(prod_sem1);
		}
		else if(type1==2){
			down(prod_sem2);
		}				
		
		down(pmtx_sem);
		ra++;
		ra = ra% 3;
		sleep(rate);
		if(type1==1&&buff_ptr[0]==0){
			switch(ra+1){
			case 1:
			printf("producer put: 1(纸、胶水)to Buffer[0]\n");
			break;
			case 2:
			printf("producer put: 2(烟草、胶水)to Buffer[0]\n");
			break;
			case 3:
			printf("producer put: 3(烟草、纸)to Buffer[0]\n");
			break;
			}
			buff_ptr[0] = ra + 1;
			
		}
		
		if(type1==2&&buff_ptr[1]==0){
			switch(ra+1){
			case 1:
			printf("producer put: 1(纸、胶水)to Buffer[1]\n");
			break;
			case 2:
			printf("producer put: 2(烟草、胶水)to Buffer[1]\n");
			break;
			case 3:
			printf("producer put: 3(烟草、纸)to Buffer[1]\n");
			break;
			}
			buff_ptr[1] = ra + 1;
		
		}
		up(pmtx_sem);
		if (ra == 0) up(sm1_sem);
		else if (ra == 1) up(sm2_sem);
		else if (ra == 2) up(sm3_sem);
		
		
	}
	return EXIT_SUCCESS;
}
