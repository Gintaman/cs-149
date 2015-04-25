#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <sys/time.h>

struct itimerval timer;
time_t startTime;
int timesUp = 0;

void timerHandler(int signal) 
{
	timesUp = 1;
}

float getTime() 
{
	clock_t t1, t2;
	t1 = clock();
	int i;
	for(i = 0; i < CLOCKS_PER_SEC; i++) {
		int x = 90;
	}
	t2 = clock();
	float diff = (((float)t2 - (float)t1) / 1000000.0F ) * 1000;
	return diff;
}

int main(int argc, char *argv[]) 
{
	time(&startTime);
	timer.it_value.tv_sec = 3;
	setitimer(ITIMER_REAL, &timer, NULL);
	signal(SIGALRM, timerHandler);
	srand(time(0));

	int fd[2]; //file descriptor array. fd[0] is for reading, fd[1] is for writing
	int numBytes;
	pid_t childpid; //child
	char string[] = "hello, world\n";
	char readbuffer[80];

	FILE *fhandler;
	fhandler = fopen("output.txt", "a+");
	
	pipe(fd); //create our pipe

	/*
	 * Note on the call to fork(). fork() returns a process id, but after fork has been called there are two process running -- the 
	 * parent process and the child process. fork is called ONCE, but returns TWICE. The parent process is returned the pid of the 
	 * spawned child, but the child is returned 0. By checking the return value, a process can determine whether is the child or 
	 * the parent. So if childpid == 0, then the process is the child, otherwise the process is the parent.
	 */
	if((childpid = fork()) == -1) { //error forking child process 
		perror("fork error\n");
		exit(1);
	}

	while(!timesUp) {
		//BEGIN FIRST PROCESS---------------------------------------------------------------------------------------
		//child process WRITES to parent. close unused READ end. 
		sleep(rand()%3);
		int i = 0;
		
		if(childpid == 0) { //current process is the CHILD process.
			close(fd[0]); //child process closes input side of pipe
			write(fd[1], string, (strlen(string)+1));
			exit(0);
		} 
		else { //parent process READS from child. close unused WRITE end
			close(fd[1]); //parent process closes output side of pipe
			numBytes = read(fd[0], readbuffer, sizeof(readbuffer));
			printf("received string: %s", readbuffer);
			//fprintf(fhandler, "some text: %s\n", readbuffer);
		}
		//END FIRST PROCESS-----------------------------------------------------------------------------------------
	}
	
	fclose(fhandler);
	exit(0);
}
