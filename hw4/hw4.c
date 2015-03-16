#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "segments.h"
void bestFit()
{
	createSegments();
	createProcesses();
	int i;
	int offset;
	int index;
	struct Process* current = head;
	struct Process* current2 = head; //only used for printing tests
	for(index = 0; index < 100; index += offset) { //initialize segments with processes
		if(index > 0)
			current = current->next;
		if(index + current->size > 100)
			break;
		offset = 0;
		while(offset < current->size) { //
			segments[index+offset]->freeSpaces = 0;
			segments[index+offset]->name = current->name;
			segments[index+offset]->offset = current->size; //added the -1. not sure. 
			segments[index+offset]->duration = current->duration;
			offset++;
		}		
	}
	current2 = head;
	/*while(current2 != NULL) {
		printf("Process: %c, Duration: %d, Size: %d\n", current2->name, current2->duration, current2->size);
		current2 = current2->next;
	}*/
	printSegments();
	int x, y, z;
	int count;
	int step; 
	int processOffset;
	char processFinished;
	char processStarted;
	char currentProcess;


	for(i = 0; i < 60; i++) { //for each time quanta
		step = 1;
		processFinished = 0;
		processOffset = 0;

		/*
		 * Decrement each segment's process's duration, update the name of process running.
		 */
		for(x = 99; x >= 0; x -= processOffset) { //decrement each process
			segments[x]->duration = segments[x]->duration - 1;
			if(x < 99) {
				currentProcess = segments[x+1]->name;
			}

			if(segments[x]->duration == 0 && x - processOffset >= 0) { //swap out old process	
				processOffset = segments[x]->offset;//processOffset = 11;			
				for(z = 0; z < processOffset; z++) {
					if(processFinished == 0)
						processFinished = segments[x]->name;
					if(x-z >= 0) {
						segments[x-z]->name = '.';
						segments[x-z]->duration = 0;
					}
				}	
				if(processFinished && processFinished != '.') {
					printf("Process %c finished and swapped out.\n", processFinished);
					processFinished = 0;
				}
			} else {
				step = 1;
				processOffset = 1;
			}	
		}

		/*
		 * Calculate the number of free spaces
		 */
		step = 1;
		for(x = 99; x >= 0; x--) { //calculate number of free spaces
			if(segments[x]->name == '.')	
				segments[x]->freeSpaces = step++;
			else
				step = 1;
		}
		//swapping process out seems ok
		//at this point, current is the first process still in the queue
		processOffset = 0;
		x = 0;
		while(x + processOffset <= 100) {
			//printf("%d\n", x);
			processStarted = 0;
			processOffset = 1;
			if(current->size <= segments[x]->freeSpaces) {
				processOffset = current->size;
				for(z = 0; z < processOffset; z++) {
					if(processStarted == 0) 
						processStarted = current->name;
					segments[x+z]->name = processStarted;
					segments[x+z]->freeSpaces = 0;
					segments[x+z]->duration = current->duration;
				}
				if(processStarted && processStarted != '.') {
					printf("Process %c started and swapped in.\n", processStarted);
					processStarted = 0;
					current = current->next;
				}
			} 
			
			
			x += processOffset;

		}
		printf("Time: %d ==> ", i);
		printSegments();
	}
	
}


int main(int argc, char* argv[])
{
	int i;
	for(i = 0; i < 5; i++) {
		printf("-----------------------------------------------------------------------------------------------------------------------\n");
		printf("Run #%d\n", i+1);
		bestFit();
		printf("------------------------------------------------------------------------------------------------------------------------\n\n\n");
	}
	return 0;
}
