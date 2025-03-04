
#include "pctl.h"
int main(int argc, char *argv[]) {
    int num=3,pid1,pid2,status;
    signal(SIGINT, (sighandler_t)sigcat);
    while(num--)
    {
    	pid1=fork();
    	if(pid1<0)
    	{
    		printf("Create Process fail!\n");
        	exit(EXIT_FAILURE);
    	}
    	else if(pid1==0)
    	{
    		printf("I am %d,my father is %d\n",getpid(),getppid());
    		pause();
    		execlp("/bin/ls", "ls", NULL);//pid1 use ls
    	}
    	else//parent
    	{
    		pid2=fork();//build pid2 in parent
    		if(pid2<0)
    		{
    			printf("Create Process fail!\n");
       		 	exit(EXIT_FAILURE);
    		}
    		else if(pid2==0)
    		{
    			printf("I am %d,my father is %d\n",getpid(),getppid());
    			status = execlp("/bin/ps", "ps", NULL);//pid2 use ps
    			
    		}
    		else//wait pid2 finish , then finish pid1 ,so ps is before ls
    		{
    			waitpid(pid2,&status,0);
    			kill(pid1, SIGINT);
    		}
    		
    	}
    
    	sleep(3);
    }
    return EXIT_SUCCESS;
}

