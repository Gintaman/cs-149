

struct Segment {
	int offset;	
	char name;
	int duration;
	int index;
	int freeSpaces;
};

struct Process {
	struct Process* next;
	struct Process* prev;
	int size;
	int duration;
	char name;
};

struct Process* head = NULL;
struct Segment* segments[100];

struct Process* createProcess()
{
	struct Process* n = (struct Process*) malloc(sizeof(struct Process));
	return n;
}

struct Segment* createSegment()
{
	struct Segment* s = (struct Segment*) malloc(sizeof(struct Segment));
	return s;
}


void addProcessTail(struct Process* current)
{
	struct Process* temp = head;
	if(head == NULL) {
		head = current;
		return;
	}
	while(temp->next != NULL)
		temp = temp->next;
	temp->next = current;
	current->prev = temp;
}


void printSegments()
{
	int i;
	for(i = 1; i <= 100; i++) {
		printf("%c", segments[i-1]->name);
	}
	printf("\n");
}

void createSegments()
{
	int i;
	for(i = 0; i < 100; i++) {
		struct Segment* newSegment = createSegment();
		newSegment->offset = 0;
		newSegment->duration = 0;
		newSegment->index = i;
		newSegment->name = '.';
		segments[i] = newSegment;
	}
}

void createProcesses()
{
	int i;
	for(i = 0, srand(time(0)); i < 200; i++) {
		struct Process* newProcess = createProcess(); 	
		newProcess->size = rand() % 4;
		switch(newProcess->size) {
			case(0): newProcess->size = 5; break;
			case(1): newProcess->size = 11; break;
			case(2): newProcess->size = 17; break;
			case(3): newProcess->size = 31; break;
		} 
		
		newProcess->name = (i % 26) + 65;
		newProcess->duration = (rand() % 5) + 1;
		addProcessTail(newProcess);
	} 
}
