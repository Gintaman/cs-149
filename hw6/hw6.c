#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <assert.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/time.h>

#define BUFFER_SIZE	100 
#define READ_END 0
#define WRITE_END 1

fd_set readFiles, fdSeter;
struct itimerval timer;
struct timeval timeout;
time_t startTime;
struct timeval start;
char buffer[BUFFER_SIZE];
char buffer2[BUFFER_SIZE];
FILE *fhandler;
int messageCount = 1;
int timesUp = 0;
int pipes[5][2];

void timerHandler(int signal) 
{
	assert(signal == SIGALRM);
	timesUp = 1;
	exit(0);
}

void readFromPipe(int readPipeEnd, int childPipe)
{
	struct timeval currentTime;
	gettimeofday(&currentTime, NULL);
	float now = (float) ((currentTime.tv_sec - start.tv_sec) + (currentTime.tv_usec - start.tv_usec)/1000000.);
	read(readPipeEnd, buffer, BUFFER_SIZE);
	if(childPipe == 4) {
		fprintf(fhandler, "%5.3f: You typed: %s", now, buffer);
	}
	else {
		fprintf(fhandler, "%5.3f: %s\n", now, buffer);
	}
}

void writeToPipe(int *pipeDescriptor)
{
	close(pipeDescriptor[READ_END]);
	write(pipeDescriptor[WRITE_END], buffer, BUFFER_SIZE);
}

int main()
{
	fhandler = fopen("output.txt", "a+");

	time(&startTime);
	timer.it_value.tv_sec = 30;
	setitimer(ITIMER_REAL, &timer, NULL);
	signal(SIGALRM, timerHandler);
	gettimeofday(&start, NULL);
	srand(time(0));

	FD_ZERO(&fdSeter);
	FD_SET(0, &fdSeter);

	pid_t pid;
	int selectPipeNumber;
	int i;
	int childNames[5];

	for(i = 0; i < 5; i++) { 
		if(pipe(pipes[i]) == -1) {
			perror("Pipe error");
			exit(1);
		}
		FD_SET(pipes[i][0], &fdSeter);		
		if((pid = fork()) == -1) {
			perror("Fork error");
			exit(1);
		}
		if(pid == 0) //each child will have its own i 
			break;
	}

	while(!timesUp){
		if(pid > 0){ //parent
			readFiles = fdSeter;
			if((selectPipeNumber = select(FD_SETSIZE, &readFiles, NULL, NULL, NULL)) == -1) {
				exit(1);
			}
			else if(selectPipeNumber == 0) {
				perror("Nothing read");
			}
			else {
				for(i = 0; i < 5; i++) {
					if(FD_ISSET(pipes[i][0], &readFiles)) {
						readFromPipe(pipes[i][0], i);
						struct timeval currentTime;
						gettimeofday(&currentTime, NULL);
						float now = (float) ((currentTime.tv_sec - start.tv_sec) + (currentTime.tv_usec - start.tv_usec)/1000000.);
						fprintf(fhandler, "%6.3f Parent \n", now);
					}
				}
			}
		}
		else {
			if(i == 4) {
				fgets(buffer2, BUFFER_SIZE, stdin);
				snprintf(buffer, BUFFER_SIZE, "%s", buffer2);
				writeToPipe(pipes[i]);
			}
			else {
				readFiles = fdSeter;
				sleep(rand()%3);
				sprintf(buffer, "Child %d message %d", (i+1), messageCount++);
				writeToPipe(pipes[i]);
			}
		}
	}	
	exit(0);
	fclose(fhandler);	
}
