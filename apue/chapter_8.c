#include "m_stdio.h"
#include <sys/wait.h>
#include <pthread.h>
#include <limits.h>

static void
print_exit
(
	int status
)
{
	if(WIFEXITED(status)){
		log_d("Normal exit:statue = %d",WEXITSTATUS(status));
	}else if(WIFSIGNALED(status)){
		log_d("Abnormal exit:sig = %d(%s)",
			WTERMSIG(status),
			WCOREDUMP(status)?"Core file":"");
	}else if(WIFSTOPPED(status)){
		log_d("Child stop:sig = %d",WSTOPSIG(status));
	}else{
		log_w("Unknow status.");
	}
}

pthread_t thread_id;
int running = 0;

void *thread_func(void * args)
{
	log_d("Thread start.");
    FILE *f = popen("tail -f ~/data/setenv.sh", "r");
    if(!f)
    {
        return ((void*)0);
    }
    char buf[200];
    while(running){
		log_d("running....................");
        bzero(buf,200);
        if(fgets(buf,200,f)){
        	log_d("lind:%s\n",buf);
        }else{
			log_e("fgets() error.");
        }
    }

    if(f){
        fclose(f);
        f = NULL;
    }
	log_d("Thread end.");

    return ((void*)0);
}



static void
main_1
(
	void
)
{
	pid_t pid;
	int status;

	if((pid = fork()) < 0){
		log_d("fork fail.");
	}else if (pid == 0){ // Child
		exit(7);
	}

	if(wait(&status) == pid){
		print_exit(status);
	}else{
		log_w("wait for pid error.");
	}

	if((pid = fork()) < 0){
		log_d("fork fail.");
	}else if (pid == 0){ // Child
		abort();
	}

	if(wait(&status) == pid){
		print_exit(status);
	}else{
		log_w("wait for pid error.");
	}

	if((pid = fork()) < 0){
		log_d("fork fail.");
	}else if (pid == 0){ // Child
		status /= 0;
	}

	if(wait(&status) == pid){
		print_exit(status);
	}else{
		log_w("wait for pid error.");
	}

}

void main_2(void)
{
	running = 1;
    if(pthread_create(&thread_id,
                    NULL,
                    thread_func,
                    NULL) != 0){
        log_d("Thread create fail.\n");
    }

	sleep(3);

	//running = 0;
	if(pthread_cancel(thread_id) == 0){
		log_d("pthread_cancel() success.");
	}else{
		log_e("pthread_cancel() fail.");
	}
	log_d("Wait for thread end.");
	pthread_join(thread_id, NULL);
	log_d("Thread end return.");
	log_d("exit");
}

void main_3(void)
{
	log_d("login = %s,uid = %d,euid = %d,nice = %d",getlogin(),getuid(),geteuid(),nice(NZERO));
	log_d("pid = %d,pgrp = %d,pgid = %d",getpid(),getpgrp(),getpgid(10));
}

int main(int arg, char *args[])
{
	log_init(LOG_D,FALSE,NULL);
	//main_1();
	//main_2();
	main_3();
	exit(0);
}
