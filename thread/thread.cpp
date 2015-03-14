#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

sem_t sem;
pthread_t tid;

static void * thread_function(void * arg){
	sleep(5);
	sem_post(&sem);
}

int main(){
	
	sem_init(&sem,0,0);
	pthread_create(&tid,NULL,thread_function,NULL);

	// 等待子进程释放信号
	sem_wait(&sem);
	printf("wait return,thread finished.");
	
	return(0);
}
