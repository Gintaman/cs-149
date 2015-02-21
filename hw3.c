#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <sys/time.h>

#define GS 2
#define RS 1
#define EE 0

struct Student {
    int status; //0-2 inclusive
    int id; //0-2 inclusive
    int section; //0-2 inclusive
    int arrival; //time student arrives. 0-199 inclusive
    int processTime;
    struct Student* next; 
    struct Student* previous;
};

struct Student* gsHead; //create 3 queues
struct Student* rsHead;
struct Student* eeHead;

struct Student* createStudent(int id)
{
    struct Student* s = (struct Student*) malloc(sizeof(struct Student)); 
    s->status = rand()%3;
    s->section = rand()%3;
    s->arrival = rand()%120;
    s->id = id;
    s->next = NULL;
    s->previous = NULL;    
    if(s->status == 2) 
        s->processTime = (rand()%2)+1;    
    else if(s->status == 1)
        s->processTime = 1 + ((rand()%3)+1);
    else 
        s->processTime = 2 + ((rand()%4)+1);    
    return s;
}

/*
 *  Add a student to the head of its respective queue 
 */
void addFirst(struct Student* current) 
{
    if(current->status == 2) {
        if(gsHead == NULL) {
            gsHead = current;
            return;
        }
        gsHead->previous = current;
        current->next = gsHead;
        gsHead = current;
    }        
    else if(current->status == 1) {
        if(rsHead == NULL) {
            rsHead = current;
            return;
        }
        rsHead->previous = current;
        current->next = rsHead;
        rsHead = current;
    }
    else {
        if(eeHead == NULL) {
            eeHead = current;
            return;
        }
        eeHead->previous = current;
        current->next = eeHead;
        eeHead = current;
    }  
}

/*
 *  Add a student to the tail of its respective queue
 */
void addLast(struct Student* current)
{
    if(current->status == 2) {
        struct Student* temp = gsHead;
        if(gsHead == NULL) {
            gsHead = current;
            return;
        }
        while(temp->next != NULL) temp = temp->next;          
        temp->next = current;
        current->previous = temp;
    }      
    else if(current->status == 1) {
        struct Student* temp = rsHead;
        if(rsHead == NULL) {
            rsHead = current;
            return;
        }
        while(temp->next != NULL) temp = temp->next;          
        temp->next = current;
        current->previous = temp;
    }
    else {
        struct Student* temp = eeHead;
        if(eeHead == NULL) {
            eeHead = current;
            return;
        }
        while(temp->next != NULL) temp = temp->next;          
        temp->next = current;
        current->previous = temp;
    }
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

int main(int argc, char *argv)
{
    srand(time(NULL));
    
    int i;
    for(i = 0; i < 75; i++) { //this creates our 75 students
        addFirst(createStudent(i));    
    }

    printf("Graduating Seniors: ");
    s_print(gsHead);
    printf("\nRegular Seniors: ");
    s_print(rsHead);
    printf("\nEveryone Else: ");
    s_print(eeHead);
    return 0;    
}
