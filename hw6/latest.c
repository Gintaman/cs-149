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

fd_set inputs, inputfds;
struct itimerval timer;
time_t startTime;
int timesUp = 0;
int fd[5][2];
char write_msg[BUFFER_SIZE] = "child 1\n";
char read_msg[BUFFER_SIZE];
char buffer[BUFFER_SIZE];
FILE *fhandler;

void timerHandler(int signal) 
{
	timesUp = 1;
}

void readFromPipe(int readEnd, int pid)
{
	read(readEnd, buffer, BUFFER_SIZE);
	fprintf(fhandler, "%s\n", buffer);
}

void writeToPipe(int fd[]) 
{
	write(fd[1], buffer, 128);
}

int main() 
{
	fhandler = fopen("output.txt", "a+");

	time(&startTime);
	timer.it_value.tv_sec = 3;
	setitimer(ITIMER_REAL, &timer, NULL);
	signal(SIGALRM, timerHandler);
	srand(time(0));

	char buffer[128];
	int result, nread;

	struct timeval timeout;

	pid_t pid;
	int fd[2];

	FD_ZERO(&inputs);
	FD_SET(0, &inputs);


	if(pipe(fd) == -1) {
		perror("pipe error");
		exit(1);
	}

	pid = fork();

	inputfds = inputs;
	timeout.tv_sec = 5;
	timeout.tv_usec = 0;
	while(!timesUp) {
		if(pid > 0) { //parent process
			result = select(FD_SETSIZE, &inputfds,
					NULL, NULL, NULL);

			if(result == -1) {
				perror("select error: ");
				exit(1);
			}
			else if(result == 0) {
				printf("its fucking nothing\n");
				fflush(stdout);
			}
			else {
				if(FD_ISSET(fd[0], &inputfds)) {
					readFromPipe(fd[0], 0);
				}
			}
		}
		else {
			int i = 0;
			sleep(rand()%3);
			sprintf(buffer, "test string\n");
			writeToPipe(fd);
		}
	}
	fclose(fhandler);
	exit(0);
}
