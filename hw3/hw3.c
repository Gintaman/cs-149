#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <sys/time.h>
#include <string.h>

#define GS 2
#define RS 1
#define EE 0
#define SECTION_SIZE 20
#define QUEUE_COUNT 3
#define DURATION 120
#define MAX_STUDENTS 75

struct Student {
    int status; //0-2 inclusive
    int id; //0-2 inclusive
    int section; //0-2 inclusive
    int arrival; //time student arrives. 0-199 inclusive
    int processTime;
    struct Student* next; 
    struct Student* previous;
};

int numStudents = 1;
int current = 0; //0 for ee, 1 for rs, 2 for gs
int droppedCount = 0;

int turnAroundTimes[76];

struct Student* gsHead = NULL; //create 3 queues
struct Student* rsHead = NULL;
struct Student* eeHead = NULL;
sem_t gsSem; //semaphores for each QUEUE
sem_t rsSem;
sem_t eeSem;
sem_t emptyGS;
sem_t fullGS;
sem_t waitForGS;
sem_t waitForRS;
sem_t waitForGSEE;
struct Student* section1[SECTION_SIZE]; //arrays simulating each SECTION
struct Student* section2[SECTION_SIZE];
struct Student* section3[SECTION_SIZE];
pthread_mutex_t studentMutex; //mutexes for each SECTION
pthread_mutex_t rsMutex;
pthread_mutex_t eeMutex;
pthread_mutex_t gsQueueMutex;
pthread_mutex_t rsQueueMutex;
pthread_mutex_t eeQueueMutex;
int gsCount = 0; //number of students in the gsQueue
int rsCount = 0; //num students in rsQueue
int eeCount = 0; //)veryone else
int id = 0; //global variable for creating id's
int waitCount = 0;

int section1size = 0;
int section2size = 0;
int section3size = 0;

int gsIn = 0, gsOut = 0;
int gsCurrentStudentId = 0;

time_t startTime;
struct itimerval timer;
int timesUp = 0; //1 means time is up

struct Student* createStudent()
{
    struct Student* s = (struct Student*) malloc(sizeof(struct Student)); 
    s->status = rand()%3;
    s->section = rand()%4 + 1; 
    s->id = ++id;
    s->next = NULL;
    s->previous = NULL;    
    if(s->status == 2) { 
        s->processTime = (rand()%2)+1;  
    }
    else if(s->status == 1) {
        s->processTime = 1 + ((rand()%3)+1);
    }
    else {
        s->processTime = 2 + ((rand()%4)+1);
    }
    return s;
}

/*
 *  Add a student to the head of its respective queue 
 */
void addFirst(struct Student* current) 
{
    if(current->status == 2) {
        if(gsHead == NULL) {
        	gsCount++;
            gsHead = current; 
            return;
        }
        gsHead->previous = current;
        current->next = gsHead;
        gsHead = current;
        gsCount++;
        
    }        
    else if(current->status == 1) {
        if(rsHead == NULL) {
        	rsCount++;
            rsHead = current;
            return;
        }
        rsHead->previous = current;
        current->next = rsHead;
        rsHead = current;
        rsCount++;
    }
    else {
        if(eeHead == NULL) {
            eeHead = current;
            eeCount++;
            return;
        }
        eeHead->previous = current;
        current->next = eeHead;
        eeHead = current;
        eeCount++;
    }  
}

/*
 *  Add a student to the tail of its respective queue
 */
void addLast(struct Student* current)
{
    time_t now;
    time(&now);
    double elapsed = difftime(now, startTime);
    int min = 0;
    int sec = (int) elapsed;
    current->arrival = sec;
    if(sec >= 60) {
        min++;
        sec -= 60;
    }
    char studentStatus[30];
    char studentSection[30];
    if(current->section == 1)
        strcpy(studentSection, "Section 1");
    else if(current->section == 2)
        strcpy(studentSection, "Section 2");
    else if(current->section == 3)
        strcpy(studentSection, "Section 3");
    else
        strcpy(studentSection, "Any Section");

    if(current->status == 2)
        strcpy(studentStatus, "Graduating Senior");
    else if(current->status == 1)
        strcpy(studentStatus, "Regular Senior");
    else
        strcpy(studentStatus, "Everyone Else");
    printf("Time: %1d:%02d   |  Student Arrived | ID: %d | Status: %s | Section: %s\n", min, sec, current->id, studentStatus, studentSection);
    if(current->status == 2) {
        struct Student* temp = gsHead;
        if(gsHead == NULL) {
            gsHead = current;
            gsCount++;
            return;
        }
        gsCount++;
        while(temp->next != NULL) temp = temp->next;          
        temp->next = current;
        current->previous = temp;
    }      
    else if(current->status == 1) {
        struct Student* temp = rsHead;
        if(rsHead == NULL) {
            rsHead = current;
            rsCount++;
            return;
        }
        rsCount++;
        while(temp->next != NULL) temp = temp->next;          
        temp->next = current;
        current->previous = temp;
    }
    else {
        struct Student* temp = eeHead;
        if(eeHead == NULL) {
            eeHead = current;
            eeCount++;
            return;
        }
        eeCount++;
        while(temp->next != NULL) temp = temp->next;          
        temp->next = current;
        current->previous = temp;
    }
}

struct Student* removeGSHead(struct Student* head)
{
    struct Student* removed = gsHead;
    gsHead = gsHead->next;	
    return removed;
}

struct Student* removeRSHead(struct Student* head)
{
    struct Student* removed = rsHead;
    rsHead = rsHead->next;	
    return removed;
}

struct Student* removeEEHead(struct Student* head)
{
    struct Student* removed = eeHead;
    eeHead = eeHead->next;	
    return removed;
}

/*
 *  Prints out the queue.
 *  Give it parameter gsHead, rsHead, or eeHead to specify which queue
 */
void s_print(struct Student* head)
{
    struct Student* temp = head;
    while(temp != NULL) {
        printf("\n(ID: %d, Arrival Time: %d, Process Time: %d)", temp->id, temp->arrival, temp->processTime);
        temp = temp->next;
    }
    printf("\n");
}

void timerHandler(int signal)
{
    timesUp = 1;    
    sem_post(&gsSem);
    printf("Time's up\n");
}

void *processEE()
{
    sem_wait(&eeSem);
	if(gsCount > 0)
        sem_wait(&waitForGSEE);//bookmark
    if(rsCount > 0)
        sem_wait(&waitForRS);
	
    if(!timesUp) {
        pthread_mutex_lock(&eeQueueMutex);    
        struct Student* currentStudent = removeEEHead(eeHead);
        sleep(currentStudent->processTime);
        int sectionNumber = currentStudent->section;

        time_t now;
        time(&now);
        double elapsed = difftime(now, startTime);
        int min = 0;
        int sec = (int) elapsed;

        turnAroundTimes[currentStudent->id] = (sec - currentStudent->arrival) + currentStudent->processTime;

        if(sec >= 60) {
            min++;
            sec -= 60;
        }

        

        //bookmark
        //sem_wait(&waitFor)
        switch(sectionNumber) {
            case 1: 
            	if(section1size < SECTION_SIZE) {
            		section1[section1size++] = currentStudent;
            		//printf("Status: %d, Section: %d ---", currentStudent->status, currentStudent->section);
                    printf("Time: %1d:%02d   |  ", min, sec);
                	printf("Student %d enrolled into Section 1\n", currentStudent->id);
                	eeCount--;
            	}    
                else {
                    printf("Time: %1d:%02d   |  ", min, sec);
                    printf("Student %d dropped.\n", currentStudent->id);
                }       	
                break;
            case 2: 
            	if(section2size < SECTION_SIZE) {
            		section2[section2size++] = currentStudent;
            		//printf("Status: %d, Section: %d ---", currentStudent->status, currentStudent->section);
                    printf("Time: %1d:%02d   |  ", min, sec);
                	printf("Student %d enrolled into Section 2\n", currentStudent->id);
                	eeCount--;
            	}
                else {
                    printf("Time: %1d:%02d   |  ", min, sec);
                    printf("Student %d dropped.\n", currentStudent->id);
                }
                break;
            case 3:
            	if(section3size < SECTION_SIZE) {
            		section3[section3size++] = currentStudent;
            		//printf("Status: %d, Section: %d ---", currentStudent->status, currentStudent->section);
                    printf("Time: %1d:%02d   |  ", min, sec);
                	printf("Student %d enrolled into Section 3\n", currentStudent->id);
                	eeCount--;
            	}       
                else {
                    printf("Time: %1d:%02d   |  ", min, sec);
                    printf("Student %d dropped.\n", currentStudent->id);
                }  
            	break; 
            case 4: 
                if(section1size < SECTION_SIZE) {
                    section1[section1size++] = currentStudent; 
                    //printf("Status: %d, Section: %d ---", currentStudent->status, currentStudent->section);
                    printf("Time: %1d:%02d   |  ", min, sec);
                    printf("Student %d enrolled into Section 1\n", currentStudent->id);  
                    eeCount--; 
                }
                else if(section2size < SECTION_SIZE) {
                    section2[section2size++] = currentStudent;  
                    //printf("Status: %d, Section: %d ---", currentStudent->status, currentStudent->section);
                    printf("Time: %1d:%02d   |  ", min, sec);
                    printf("Student %d enrolled into Section 2\n", currentStudent->id);  
                    eeCount--;
                }
                else if(section3size < SECTION_SIZE) {
                	section3[section3size++] = currentStudent;
                	//printf("Status: %d, Section: %d ---", currentStudent->status, currentStudent->section);
                    printf("Time: %1d:%02d   |  ", min, sec);
                	printf("Student %d enrolled into Section 3\n", currentStudent->id);
                	eeCount--;
                }
                else {
                    printf("Time: %1d:%02d   |  ", min, sec);
                    printf("Student %d dropped.\n", currentStudent->id);
                	//addLast(currentStudent); //MAYBE THIS FIXES ISSUE. STUDENTS NEED TO BE READDED TO QUEUE
                }
                break;
        }
        pthread_mutex_unlock(&eeQueueMutex);
    }
}

void *processRS()
{
    sem_wait(&rsSem);
	if(gsCount > 0)
        	sem_wait(&waitForGS);
    

    if(!timesUp) {
        pthread_mutex_lock(&rsQueueMutex);    
        struct Student* currentStudent = removeRSHead(rsHead);
        sleep(currentStudent->processTime);
        int sectionNumber = currentStudent->section;
        time_t now;
        time(&now);
        double elapsed = difftime(now, startTime);
        int min = 0;
        int sec = (int) elapsed;
        turnAroundTimes[currentStudent->id] = (sec - currentStudent->arrival) + currentStudent->processTime;
        if(sec >= 60) {
            min++;
            sec -= 60;
        }

        
         //if there are seniors, we wait for them
        switch(sectionNumber) {
            case 1: 
            	if(section1size < SECTION_SIZE) {
            		section1[section1size++] = currentStudent;
                    printf("Time: %1d:%02d   |  ", min, sec);
            		//printf("Status: %d, Section: %d ---", currentStudent->status, currentStudent->section);
                	printf("Student %d enrolled into Section 1\n", currentStudent->id);
                	rsCount--;
                	sem_post(&waitForGS);
                	sem_post(&waitForGSEE);
                	sem_post(&waitForRS);
            	}           	
                else {
                    printf("Time: %1d:%02d   |  ", min, sec);
                    printf("Student %d dropped.\n", currentStudent->id);
                }
                break;
            case 2: 
            	if(section2size < SECTION_SIZE) {
            		section2[section2size++] = currentStudent;
                    printf("Time: %1d:%02d   |  ", min, sec);
            		//printf("Status: %d, Section: %d ---", currentStudent->status, currentStudent->section);
                	printf("Student %d enrolled into Section 2\n", currentStudent->id);
                	rsCount--;
                	sem_post(&waitForGS);
                	sem_post(&waitForGSEE);
                	sem_post(&waitForRS);
            	}
                else {
                    printf("Time: %1d:%02d   |  ", min, sec);
                    printf("Student %d dropped.\n", currentStudent->id);
                }
                break;
            case 3:
            	if(section3size < SECTION_SIZE) {
            		section3[section3size++] = currentStudent;
                    printf("Time: %1d:%02d   |  ", min, sec);
            		//printf("Status: %d, Section: %d ---", currentStudent->status, currentStudent->section);
                	printf("Student %d enrolled into Section 3\n", currentStudent->id);
                	rsCount--;
                	sem_post(&waitForGS);
                	sem_post(&waitForGSEE);
                	sem_post(&waitForRS);
            	}          
                else {
                    printf("Time: %1d:%02d   |  ", min, sec);
                    printf("Student %d dropped.\n", currentStudent->id);
                }
            	break; 
            case 4: 
                if(section1size < SECTION_SIZE) {
                    section1[section1size++] = currentStudent; 
                    printf("Time: %1d:%02d   |  ", min, sec);
                    //printf("Status: %d, Section: %d ---", currentStudent->status, currentStudent->section);
                    printf("Student %d enrolled into Section 1\n", currentStudent->id);  
                    rsCount--; 
                    sem_post(&waitForGS);
                	sem_post(&waitForGSEE);
                	sem_post(&waitForRS);
                }
                else if(section2size < SECTION_SIZE) {
                    section2[section2size++] = currentStudent;  
                    printf("Time: %1d:%02d   |  ", min, sec);
                    //printf("Status: %d, Section: %d ---", currentStudent->status, currentStudent->section);
                    printf("Student %d enrolled into Section 2\n", currentStudent->id);  
                    rsCount--;
                    sem_post(&waitForGS);
                	sem_post(&waitForGSEE);
                	sem_post(&waitForRS);
                }
                else if(section3size < SECTION_SIZE) {
                	section3[section3size++] = currentStudent;
                    printf("Time: %1d:%02d   |  ", min, sec);
                	//printf("Status: %d, Section: %d ---", currentStudent->status, currentStudent->section);
                	printf("Student %d enrolled into Section 3\n", currentStudent->id);
                	rsCount--;
                	sem_post(&waitForGS);
                	sem_post(&waitForGSEE);
                	sem_post(&waitForRS);
                }
                else {
                    printf("Time: %1d:%02d   |  ", min, sec);
                    printf("Student %d dropped.\n", currentStudent->id);
                	//addLast(currentStudent);
                }
                break;
        }
        pthread_mutex_unlock(&rsQueueMutex);
    }
}

void *processGS()
{
    sem_wait(&gsSem); //HIGHEST PRIORITY. no need to check on other queues, just take students and enroll them.
    if(!timesUp && gsCount > 0) {
        pthread_mutex_lock(&gsQueueMutex); 
        struct Student* currentStudent = removeGSHead(gsHead);
        sleep(currentStudent->processTime);
        int sectionNumber = currentStudent->section;  
        time_t now;
        time(&now);
        double elapsed = difftime(now, startTime);
        int min = 0;
        int sec = (int) elapsed;
        turnAroundTimes[currentStudent->id] = (sec - currentStudent->arrival) + currentStudent->processTime;
        if(sec >= 60) {
            min++;
            sec -= 60;
        }

        
        switch(sectionNumber) {
            case 1: 
            	if(section1size < SECTION_SIZE) {
            		section1[section1size++] = currentStudent;
                    printf("Time: %1d:%02d   |  ", min, sec);
            		//printf("Status: %d, Section: %d ---", currentStudent->status, currentStudent->section);
                	printf("Student %d enrolled into Section 1\n", currentStudent->id);
                	gsCount--;
                	sem_post(&waitForGS);
                	sem_post(&waitForGSEE);
            	}         
                else {
                    printf("Time: %1d:%02d   |  ", min, sec);
                    printf("Student %d dropped.\n", currentStudent->id);
                }  	
                break;
            case 2: 
            	if(section2size < SECTION_SIZE) {
            		section2[section2size++] = currentStudent;
                    printf("Time: %1d:%02d   |  ", min, sec);
            		//printf("Status: %d, Section: %d ---", currentStudent->status, currentStudent->section);
                	printf("Student %d enrolled into Section 2\n", currentStudent->id);
                	gsCount--;
                	sem_post(&waitForGS);
                	sem_post(&waitForGSEE);
            	} 
                else {
                    printf("Time: %1d:%02d   |  ", min, sec);
                    printf("Student %d dropped.\n", currentStudent->id);
                }
                break;
            case 3:
            	if(section3size < SECTION_SIZE) {
            		section3[section3size++] = currentStudent;
                    printf("Time: %1d:%02d   |  ", min, sec);
            		//printf("Status: %d, Section: %d ---", currentStudent->status, currentStudent->section);
                	printf("Student %d enrolled into Section 3\n", currentStudent->id);
                	gsCount--;
                	sem_post(&waitForGS);
                	sem_post(&waitForGSEE);
            	}          
                else {
                    printf("Time: %1d:%02d   |  ", min, sec);
                    printf("Student %d dropped.\n", currentStudent->id);
                }
            	break; 
            case 4: 
                if(section1size < SECTION_SIZE) {
                    section1[section1size++] = currentStudent; 
                    printf("Time: %1d:%02d   |  ", min, sec);
                    //printf("Status: %d, Section: %d ---", currentStudent->status, currentStudent->section);
                    printf("Student %d enrolled into Section 1\n", currentStudent->id);  
                    gsCount--; 
                    sem_post(&waitForGS);
                    sem_post(&waitForGSEE);
                }
                else if(section2size < SECTION_SIZE) {
                    section2[section2size++] = currentStudent;  
                    printf("Time: %1d:%02d   |  ", min, sec);
                    //printf("Status: %d, Section: %d ---", currentStudent->status, currentStudent->section);
                    printf("Student %d enrolled into Section 2\n", currentStudent->id);  
                    gsCount--;
                    sem_post(&waitForGS);
                    sem_post(&waitForGSEE);
                }
                else if(section3size < SECTION_SIZE) {
                	section3[section3size++] = currentStudent;
                    printf("Time: %1d:%02d   |  ", min, sec);
                	//printf("Status: %d, Section: %d ---", currentStudent->status, currentStudent->section);
                	printf("Student %d enrolled into Section 3\n", currentStudent->id);
                	gsCount--;
                	sem_post(&waitForGS);
                	sem_post(&waitForGSEE);
                }
                else {
                    printf("Time: %1d:%02d   |  ", min, sec);
                    printf("Student %d dropped.\n", currentStudent->id);
                	//addLast(currentStudent);
                }
                break;
        } 
        pthread_mutex_unlock(&gsQueueMutex);
        //after a student is enrolled, we increment and unlock the semaphore again.
    } 
}

void *gsQueue() //Graduating Seniors queue thread
{
    do {
        processGS();    
    } while(!timesUp);
    return NULL;
}



void *rsQueue(void *param)
{
    do {
        processRS();
    } while(!timesUp);
    return NULL;
}

void *eeQueue(void *param)
{
 	do {
 		processEE();
 	} while(!timesUp);
 	return NULL;
}

void *studentArrives() //add student to queue
{
    pthread_mutex_lock(&studentMutex);
    struct Student* arrivingStudent = createStudent();
    addLast(arrivingStudent); //student is added to its correct queue
    if(arrivingStudent->status == 2)
    	sem_post(&gsSem); 
    else if(arrivingStudent->status == 1)
    	sem_post(&rsSem);
    else
    	sem_post(&eeSem);
    pthread_mutex_unlock(&studentMutex); 
}

void *student()
{
    sleep(rand()%DURATION);
    studentArrives(); //student arrives at random time
}



int main(int argc, char *argv)
{
    srand(time(0)); //set up our random number generator. only call this once.
    pthread_mutex_init(&studentMutex, NULL);
    pthread_mutex_init(&rsMutex, NULL);
    pthread_mutex_init(&eeMutex, NULL);
    pthread_mutex_init(&gsQueueMutex, NULL);
    pthread_mutex_init(&rsQueueMutex, NULL);
    pthread_mutex_init(&eeQueueMutex, NULL);
    sem_init(&gsSem, 0, 0);
    sem_init(&rsSem, 0, 0);
    sem_init(&eeSem, 0, 0);
    sem_init(&waitForGS, 0, 0);
    sem_init(&waitForGSEE, 0, 0);
    sem_init(&waitForRS, 0, 0);
    time(&startTime);
    
    timer.it_value.tv_sec = DURATION;
    
    setitimer(ITIMER_REAL, &timer, NULL);

    pthread_t gsQueueThreadId; 
    pthread_attr_t gsAttr;
    pthread_attr_init(&gsAttr);
    pthread_create(&gsQueueThreadId, &gsAttr, gsQueue, NULL);

    pthread_t rsQueueThreadId;
    pthread_attr_t rsAttr;
    pthread_attr_init(&rsAttr);
    pthread_create(&rsQueueThreadId, &rsAttr, rsQueue, NULL);

    pthread_t eeQueueThreadId;
    pthread_attr_t eeAttr;
    pthread_attr_init(&eeAttr);
    pthread_create(&eeQueueThreadId, &eeAttr, eeQueue, NULL);

    int i;

    for(i = 0; i < MAX_STUDENTS; i++) {
        pthread_t studentThreadId;
        pthread_attr_t studentAttr;
        pthread_attr_init(&studentAttr);
        pthread_create(&studentThreadId, &studentAttr, student, NULL);
    }
    
    signal(SIGALRM, timerHandler);
    pthread_join(gsQueueThreadId, NULL); //all student threads will hit this code eventually.

    char studentStatus[30];
    printf("Number of students who did not manage to enroll: %d\n", MAX_STUDENTS - (section1size + section2size + section3size));
    printf("\n");
    printf("---------- Students enrolled in section 1 ----------\n");
    for(i = 0; i < section1size; i++) {
        if(section1[i]->status == 2)
            strcpy(studentStatus, "Graduating Senior");
        else if(section1[i]->status ==1)
            strcpy(studentStatus, "Regular Senior");
        else
            strcpy(studentStatus, "Everyone Else");
        printf("Student: %d | Status: %s\n", section1[i]->id, studentStatus);
    }
    printf("---------- Students enrolled in section 1 ----------\n");
    printf("\n");
    printf("---------- Students enrolled in section 2 ----------\n");
    for(i = 0; i < section2size; i++) {
        if(section2[i]->status == 2)
            strcpy(studentStatus, "Graduating Senior");
        else if(section2[i]->status ==1)
            strcpy(studentStatus, "Regular Senior");
        else
            strcpy(studentStatus, "Everyone Else");
        printf("Student: %d | Status: %s\n", section2[i]->id, studentStatus);
    }
    printf("---------- Students enrolled in section 2 ----------\n");
    printf("\n");
    printf("---------- Students enrolled in section 3 ----------\n");
    for(i = 0; i < section3size; i++) {
        if(section3[i]->status == 2)
            strcpy(studentStatus, "Graduating Senior");
        else if(section3[i]->status ==1)
            strcpy(studentStatus, "Regular Senior");
        else
            strcpy(studentStatus, "Everyone Else");
        printf("Student: %d | Status: %s\n", section3[i]->id, studentStatus);
    }
    printf("---------- Students enrolled in section 3 ----------\n");
    printf("\n");
    int acc = 0;
    float total;
    printf("---------- Turn Around Times ----------\n");
    for(i = 1; i < 76; i++) {
        if(turnAroundTimes[i] > 0) {
            acc += turnAroundTimes[i];
            printf("Student %d | %d\n", i, turnAroundTimes[i]);
        }
        else {
            printf("Student %d was not processed.\n", i);
        }
    }
    total = (float) acc / (float) (section1size + section2size + section3size);
    printf("AVERAGE TURN AROUND TIME: %f\n", total);
    printf("---------- Turn Around Times ----------\n");
    //printf("%d\n", section1[0]->id);
    return 0;    
}


//  represent each student as a thread
//  start with a single thread
//  USE HIS OFFICE HOURS A BASE
//  thread for each queue
//  shared data: section 
//  mutex for each section (sections are arrays or arraylists)
//  we make 75 threads. then at random times generate students? CHECK HIS OFFICEHOURS.C
//  semaphores for each queue
//  due on friday still. can turn it in on monday.
