#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ioctl.h>

#define BUFFER_SIZE 32
#define READ_END 0
#define WRITE_END 1

fd_set inputs, inputsfd;
struct itimerval timer;
struct timeval timeout;
time_t startTime;
int fd[5][2];
int timesUp = 0;
struct timeval start;
char buffer[BUFFER_SIZE];
FILE *fhandler;
pid_t pid;

void timerHandler(int signal) 
{
	timesUp = 1;
}

void readFromPipe(int readEnd)
{
	read(READ_END, buffer, BUFFER_SIZE);
	fprintf(fhandler, "%s\n", buffer);
}

void writeToPipe(int *fd)
{
	close(fd[0]);
	write(fd[0], buffer, BUFFER_SIZE);
}

int main()
{
	fhandler = fopen("output.txt", "a+");

	time(&startTime);
	timer.it_value.tv_sec = 3;
	setitimer(ITIMER_REAL, &timer, NULL);
	signal(SIGALRM, timerHandler);
	srand(time(0));

	FD_ZERO(&inputsfd);
	FD_SET(0, &inputsfd);

	int result;
	int i = 1;
	if(pipe(fd[0]) == -1) {
		perror("pipe error");
	}

	FD_SET(fd[0][0], &inputsfd);

	if((pid = fork()) == - 1) {
		perror("fork error");
	}

	if(pid > 0) {
		printf("child process\n");
	}
	else {
		fflush(stdout);
		printf("parent process\n");
	}
	while(!timesUp) {
		timeout.tv_sec = 3;
		timeout.tv_usec = 0;
		inputs = inputsfd;
		if(pid > 0) { //parent
			printf("child process\n");
			close(fd[0][WRITE_END]);
			result = select(FD_SETSIZE, &inputsfd, NULL, NULL, &timeout);	
			if(result == -1) {
				perror("select error");
			}
			else if(result == 0) {
				fflush(stdout);
				perror("nothing read");
			}
			else {
				if(FD_ISSET(fd[0][0], &inputs)) {
					printf("read from pipe\n");
					readFromPipe(fd[0][0]);
				}
			}
		}
		else {
			sleep(rand()%3);
			printf("parent process\n");
			close(fd[0][READ_END]);
			sprintf(buffer, "test string: %d\n", i++);
			printf("%s\n", buffer);
			writeToPipe(fd[0]);
					//write(fd[0][1], buffer, BUFFER_SIZE);
		}
	}
	exit(0);
	fclose(fhandler);	
}
