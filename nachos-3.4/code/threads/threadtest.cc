// threadtest.cc 
//	Simple test case for the threads assignment.
//
//	Create two threads, and have them context switch
//	back and forth between themselves by calling Thread::Yield, 
//	to illustratethe inner workings of the thread system.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "synch.h"

// Global Variables
int ValidFlag;			// Used to determine which task
int *numShouts = new int;	// Number of shouts per thread
struct Philosopher {
	int id;
	int state;

};
int *numP;
int *numM;
bool *cArray;
Semaphore **sArray;
Philosopher *pArray;
int GlobalState = 0;

int p;	//Number of Philosophers or People
bool *emptyMailBox;
bool *fullMailBox;
int m;	//Total Number of meals to be eaten or messages to be sent
Semaphore **mailboxes;
int maxMessage; //Maximum number of messages a mailbox can hold
struct MessageContent{
	int recipient;
	int sender;
	char *message;
	MessageContent();
	MessageContent(char [],int,int);
};
MessageContent::MessageContent(){
	message = "\0";
	recipient = -1;
	sender = -1;
}
MessageContent::MessageContent(char letter[],int send,int receiver){
	sender = send;
	recipient = receiver;
	message = letter;
}
MessageContent **boxArray; //Keeps track of the mailboxes

// Global Variable for Dining Philosophers - Semaphores (Entered by Dustin Rowland)
Semaphore **chopSticks;		// Array of Semaphores for Dining Philosophers
int *diningState;		// Array of states for each philosopher
int groupState;			// State of all philosophers as a group
int *numPhils = new int;	// Number of philosophers
int *numMeals = new int;
int *numChopsticks = new int;
Semaphore readyToEnter("enter", 0);	// Semaphore used for Entering Room
Semaphore readyToSit("sit", 0);	// Semaphore used for Sitting at table
Semaphore readyToLeave("leave", 0);
Semaphore readyToEat("eat", 1);


//----------------------------------------------------------------------
// SimpleThread
// 	Loop 5 times, yielding the CPU to another ready thread 
//	each iteration.
//
//	"which" is simply a number identifying the thread, for debugging
//	purposes.
//----------------------------------------------------------------------

void SimpleThread(int which)
{
	int num;
    
    	for (num = 0; num < 5; num++) {
		printf("*** thread %d looped %d times\n", which, num);
       		currentThread->Yield();
    	}
}

// code edited by MICAH-----start
// Checks if the character is between 0 and 9
bool isNumber(char input)
{
	if(input >='0' && input <='9') return true;
	else return false;	
}

// Checks if character is a period
bool isPeriod(char input)
{
	if(input == '.')return true;
	else return false;
}

// Checks if character is a negative sign
bool isNegative(char input)
{
	if(input == '-')return true;
	else return false;
}

// Checks if character is anything other than Number, Period, and Negative
bool isCharacter(char input)
{
	if(!isPeriod(input) && !isNegative(input) && !isNumber(input)) return true;
	else return false;
}

//-------------------------------------------------------------
// Uses the string input provided by the user to determine
// if it is a number (integer or decimal) or a character array 
// Assumes integer, and then sets flags accordingly as the 
// characters are processed.
//-------------------------------------------------------------
// Code edited by TRAVIS------start
char* validateInput(char* input)
{	
	// Variable Declarations
	char *output = new char[30];
	char *current = input;
	bool intFlag = true;
	bool charFlag = false;
	bool decFlag = false;
	bool negFlag = false;
	
	// Only first character can be a dash to be a negative number
	if(isNegative(input[0]) && atoi(input) <=0)
	{
		negFlag = true;
		current++;
	}
	
	// Loop through the character array and check each character
	while(*current != '\0' && !charFlag)
	{		
		if(isCharacter(*current) || isNegative(*current))
	 	{
	  		intFlag = false;
	 		charFlag = true;
	  		decFlag = false;
	  		negFlag = false;
	 	}
	 	else if(isPeriod(*current))
	 	{
	 		// Check if a number came before the decimal
	   		if(!decFlag && isNumber(*(current-1)) && isNumber(*(current+1)))
	   		{
	    			decFlag = true;
	    			intFlag = false;
	   		}
	   		// Otherwise, not a decimal by definition
	   		else 
	   		{
	    			intFlag = false;
	    			charFlag = true;
	    			decFlag = false;
	    			negFlag = false;
 	   		}
	 	}
	 	// Go to next character
	 	current++;
	}
	
	// Create Output string to be returned		
	if(negFlag)strcat(output,"Negative ");
	if(intFlag)strcat(output,"Integer ");
	if(charFlag)strcat(output,"Character ");
	if(decFlag)strcat(output,"Decimal ");
	
	return output;
}
// Code edited by MICAH AND TRAVIS--------end

//-----------------------------------------------------
// Take a thread and pick a random shout to be shouted
// for it. After shouting, cycle through a busy waiting
// loop.
//-----------------------------------------------------
// code edited by DUSTIN AND BRANDIN------------start
void shoutThread(int threadID) 
{
	// Array of Shouts
	char *shouts[] = {"Bazinga\n",
	 	"Penny...Penny...Penny\n",
		"That's right, I did it\n",
	 	"Everything beats enchanted bunny.\n",
		"Unless you have an enchanted carrot\n",
		"I don't remember that.\n",
		"Yeah Rahj, that's how it works.\n",
		"Math, Science, History, unravel in the mystery.\n",
		"BANG\n"};
	
	// Loop through each thread and print its shout
	for(int i = 0; i < *numShouts; i++)
	{
		// Declare the random variable
		int random = Random()%9;
		
		// Print the thread's shout
		printf("\nThread %d: %s\n", threadID, shouts[random]);
		
		// Busy Waiting Loop
		int busyCount = 2 + Random()%4;
		while(busyCount > 0)
		{		
			currentThread->Yield();
			busyCount--;
		}
	}
				
	currentThread->Yield();	
}
// Code edited by DUSTIN AND BRANDIN------end


//*******************************************************
// PROJECT 2
//*******************************************************
//Task 3-------------------------------------------------------
//Edited By: BRANDIN
void readMessages(int person){
	int i=0;	//the number of mail slots
	MessageContent temp;
	
	while (i<maxMessage){
		
		if (boxArray[person][i].message!="\0"){
			mailboxes[person]->P();
			printf(">User %d read: Dear %d: %s. Sincerely, %d\n",person+1,person+1,boxArray[person][i].message,boxArray[person][i].sender+1);
			//Delete the message
			boxArray[person][i] = temp;
			mailboxes[person]->V();
			currentThread->Yield();
			
			i=0;
		}
		
		i++;
	}
	emptyMailBox[person] = true;
	printf("User %d has no more messages.\n",person+1);
}

//-------------------
//Places a message inside of a mailbox if the recipient's mailbox isn't
//full. To prevent deadlock, there is a countdown.
void placeMessage(MessageContent letter){
	int deadlockCountdown = 3;
	bool added = false;
	while (deadlockCountdown > 0 && !added){
		mailboxes[letter.recipient]->P();
		for (int i=0;i<maxMessage;i++){
			if (m<=0){ 
				printf("Woops, took too long to send that! User %d discards his message.\n\n",letter.sender+1);
				mailboxes[letter.recipient]->V();
				return;
			}
			if (boxArray[letter.recipient][i].message == "\0"){ 
				boxArray[letter.recipient][i] = letter;
				added = true;
				printf("-->User %d sent: Dear %d, %s. Sincerely %d\n",letter.sender+1,letter.recipient+1,letter.message,letter.sender+1);
				emptyMailBox[letter.recipient] = false;
				break;
			}
		}
		mailboxes[letter.recipient]->V();
		if (!added){
		deadlockCountdown--; currentThread->Yield();
		}
	}
	
	if (added) {
		m--;
		printf("%d MESSAGES LEFT TO BE SENT.\n",m);
	}	//Records that a message has been sent
	else printf("User %d's mailbox is full. Deadlock Prevention Activate!\n",letter.recipient+1);
}

void composeMessage(int sender,char *mess[]){
	int letter = Random()%9;	//Random message
	int receiver = Random()%p;	//Random person
	while (receiver==sender) receiver = Random()%p;	//In case sending to itself
	//Initialize a message;
	MessageContent l(mess[letter],sender,receiver);
	
	if (m>0){ 
		printf("->User %d wrote: Dear %d, %s. Sincerely %d\n",sender+1,receiver+1,l.message,sender+1);
		placeMessage(l);
	}
	else printf("\n--|No more messages can be sent.|--\n\n");
}

void enterOffice(int person){
	printf("User %d has entered the office.\n",person+1);
}

void leaveOffice(int person){
	printf("User %d has left the office.\n",person+1);
}

void checkMailProcedure(int person){
	char *messages[] = {"Bazinga",
	 	"Penny...Penny...Penny",
		"That's right, I did it",
	 	"Everything beats enchanted bunny.",
		"Unless you have an enchanted carrot",
		"I don't remember that.",
		"Yeah Rahj, that's how it works.",
		"Math, Science, History, unravel in the mystery.",
		"BANG"};
	int num;
	while (true){ //While # of messages to be sent has not been achieved
		num = 2+Random()%4;
		enterOffice(person);
		if (m>0){
		readMessages(person);
		composeMessage(person,messages);
		}
		else printf("\n--|No more messages can be sent.|--\n\n");
		leaveOffice(person);
		for (int i=0;i<num;i++){
			currentThread->Yield();
		}
		if (m<=0) break;
		
	}
}
//Edited by: BRANDIN - End


// Project 2 - Task 1 ------------------------------
// Code edited by MICAH AND TRAVIS ---- START
// Driver Function for our input validation
void CheckInput(int ID)
{
	// Get user input
	char *input = new char[250];
	printf("\nEnter some input: ");
    	scanf("%[^\n]s",input); 
    	 
    	// Get output string   	    			
	char *output = validateInput(input);
	
	// Print output string
	printf("\n%s\n\n", output);	
}

bool AllPhilosophers(int state)
{
	
	for(int i =0; i < *numP; ++i)
		if(pArray[i].state != state)
			return false;
		return true;

}


//Code edited by Micah -- START
void PhilospherBusyWaitingAlgorithm(int threadID)
{
	int OneFlag = 0;
	if(*numP == 1) OneFlag = 1;
	printf("Philospher %d ENTERS \n", threadID);
	pArray[threadID].state = 1;
	if(AllPhilosophers(1))GlobalState = 1;
	while(GlobalState != 1) currentThread->Yield();
	printf("Philospher %d SITS \n", threadID);
	pArray[threadID].state = 2;
	if(AllPhilosophers(2))GlobalState = 2;
	while(GlobalState != 2) currentThread->Yield();
	while(*numM > 0 || pArray[threadID].state == 2)
	{
		//Trying to eat 

		//LEFT Chopstick
		while(!cArray[threadID])
			currentThread->Yield();
		printf("Philospher %d picks up LEFT chopstick \n", threadID);
		cArray[threadID] = false;
		pArray[threadID].state = 3;

		//RIGHT Chopstick
		while(!cArray[(threadID + 1) % *numP] && OneFlag == 0)
			currentThread->Yield();
		printf("Philospher %d picks up RIGHT chopstick \n", threadID);
		cArray[(threadID + 1) % *numP] = false;
		pArray[threadID].state = 4;
		int wait;
		if(*numM > 0)
		{
			*numM = *numM - 1;
			printf("Philospher %d begins eating. %d meals left \n", 		threadID, *numM);
			
			wait = 2 + Random()%4;
			while(wait > 0) 
			{	
				wait = wait - 1;
				currentThread->Yield();
			}
		}
	
		printf("Philospher %d puts down LEFT chopstick \n", threadID);
		cArray[threadID] = true;
		pArray[threadID].state = 6;
		printf("Philospher %d puts down RIGHT chopstick \n", threadID);
		cArray[(threadID + 1) % *numP] = true;
		pArray[threadID].state = 7;
		printf("Philospher %d begins thinking \n", threadID);
		pArray[threadID].state = 8;
		wait = 2 + Random()%4; //2 to 5 cycles of thinking
		while(wait > 0) 
		{	
			wait = wait - 1;
			currentThread->Yield();
		}
		printf("Philospher %d finishes thinking. %d meals left \n", threadID, *numM);
		pArray[threadID].state = 10;
		currentThread->Yield();
	}
	bool  isDone = true;
	for(int i = 0; i < *numP; ++i)
	{
		if(pArray[i].state != 10)
		isDone = false;

	}
	if(isDone)
	{
		for(int i = 0; i < *numP; ++i)
		{
			printf("Philospher %d leaves. \n", i);
			pArray[i].state = 11;
		}
	}
	currentThread->Finish();
}
//Code edited by Micah -- END


//Project 2 - Task 2
//Code edited by Dustin Rowland - START
//-------------------------------------------------------------
//DiningPhilSema
//	This function will solve the Dining Philosopher problem
//	with the use of semaphores. The problem goes through
//	a series of states. In state 0, all users enter the room.
//	In state 1, all users sit down. In state 2, all users 
//	begin eating. In state 3, all users are ready to leave.
//	There are both group states and individual states, where
//	the group state represents when all individuals are in the
//	same state. 
//-------------------------------------------------------------

void DiningPhilSema(int threadID)
{
	//Philosophers enter room
	printf("Philosopher %d has entered the room.\n", threadID);
	//Philosopher's state is now 1 after entering room
	diningState[threadID] = 1;

	//The following code uses a semaphore keep philosophers from
	//getting hasty and sitting before everyone enters the room.
	for (int i = 0; i < *numPhils; i++)
	{
		if (diningState[i] == 0)	//check if all are in room
		{
			readyToEnter.P();	//if not, wait in queue
			break;			//break after V() is called
		}
	}	
	readyToEnter.V(); 			//all are in room
	groupState = 1;				//go to state 1 to sit down

	//Philosophers sit down
	printf("Philosopher %d has sat down.\n", threadID);
	//Philosopher's state is now 2 after entering room
	diningState[threadID] = 2;

	//The following code uses a semaphore keep philosophers from
	//getting hasty and eating before everyone sits down.
	//It follows the same logic from above for entering the room.
	for (int i = 0; i < *numPhils; i++)
	{
		if (diningState[i] == 1)
		{
			readyToSit.P();
			break;
		}
	}	
	readyToSit.V();
	groupState = 2;	

	int busyCount; //This variable will be used for waiting and thinking below

	//Philosophers Dine
	while (groupState == 2)
	{
		if (*numMeals > 0)
		{
			//This semaphore allows only 1 user to pick up both chopsticks
			//This prevents a deadlock situation
			readyToEat.P();

			chopSticks[threadID]->P(); //pick up left chopstick
			printf("Philosopher %d picked up chopstick %d.\n", threadID, threadID);

			chopSticks[(threadID+1) % *numChopsticks]->P(); //pick up right chop
			printf("Philosopher %d picked up chopstick %d.\n", threadID,
				(threadID+1) % *numChopsticks);

			//The semaphore is released once both chopsticks are picked up
			readyToEat.V();

			//Ensure numMeal > 0 before getting a meal and eating
			if(*numMeals > 0)
			{
				printf("Philosopher %d is eating.\n", threadID);
				*numMeals = *numMeals - 1; //decrement num of meals
				printf("Number of meals is now %d.\n", *numMeals);

				//yield for 2-5 sec to eat
				busyCount = 2 + Random()%3;
				while(busyCount > 0)
				{
					currentThread->Yield();
					busyCount--;
				}
			}

			
			chopSticks[threadID]->V(); //put down left chopstick
			printf("Philosopher %d put down chopstick %d.\n", threadID, 
				threadID);
			chopSticks[(threadID+1) % *numChopsticks]->V(); //put down right chop
			printf("Philosopher %d put down chopstick %d.\n", threadID,
				(threadID+1) % *numChopsticks);

			//yield for 2-5 sec to think
			busyCount = 2 + Random()%3;
			printf("Philosopher %d is thinking.\n", threadID);
			while(busyCount > 0)
			{		
				currentThread->Yield();
				busyCount--;
			}

		}	

		//If no more meals, individuals move to state 3
		else
		{
			diningState[threadID] = 3;
			
			//the code below is similar to code above for entering room
			for (int i = 0; i < *numPhils; i++) 
			{
				if (diningState[i] == 2)
				{
					readyToLeave.P();
					break;
				}
			}
			groupState = 3;	//all are ready to leave
			readyToLeave.V(); 
		}
	}
	printf("Philosopher %d leaves the table.\n", threadID);

	currentThread -> Finish();
		
}
//End Editing by Dustin Rowland


// Code edited by MICAH AND TRAVIS--------end


//-------------------------------------------------------------
// Driver function for our different tasks.
//-------------------------------------------------------------
// Code edited by DUSTIN AND BRANDIN---------start
void ThreadTest()
{
	DEBUG('t', "Entering ThreadTest");
    	
    	// Create new thread
    	Thread *t = new Thread("forked thread");
    	
    	// If -A 1, run tasks 1
    	if(ValidFlag == 1)
    	{
		t->Fork(CheckInput,1);	
		currentThread->Finish();
    	}
    	// If -A 2, run tasks 2
    	else if(ValidFlag == 2)
    	{
    		// Variable Declarations
		int *numThreads = new int;
		char *input = new char[100];
		bool flag = true;
		
		// Loop until valid user input
		while(flag)
		{
			// Get number of threads			
			printf("How many threads? : ");
			scanf("%s",input);
			
			// Validate user input
			if(!strcmp(validateInput(input),"Integer "))
			{
				*numThreads = atoi(input);
				flag = false;
			}
			
			// Get number of shouts per thread
			printf("How many shouts per thread? : ");
			scanf("%s", input);
			
			// Validate user input
			if(!strcmp(validateInput(input),"Integer ") && flag == false)
			{
			 	*numShouts = atoi(input);
				break;
			}	
			printf("Incorrect User Input. Please Enter a Positive Integer.\n\n");
		}
		
		// Loop through each Thread and Shout
		for(int i = 0; i < *numThreads; i++)
		{
	   		Thread *s = new Thread("Shout Thread");
	   		s->Fork(shoutThread, i);
		}
		
		currentThread->Finish();
    	}
		else if(ValidFlag == 3)
    	{
    		// Variable Declarations
		numP = new int;
		numM = new int;
		char *input = new char[100];
		bool flag = true;
		
		// Loop until valid user input
		while(flag)
		{
			// Get number of threads			
			printf("How many Philosophers? : ");
			scanf("%s",input);
			
			// Validate user input
			if(!strcmp(validateInput(input),"Integer "))
			{
				*numP = atoi(input);
				flag = false;
			}
			
			// Get number of shouts per thread
			printf("How many meals? : ");
			scanf("%s", input);
			
			// Validate user input
			if(!strcmp(validateInput(input),"Integer ") && flag == false)
			{
			 	*numM = atoi(input);
				if(*numP > 1)
					cArray = new bool[*numP]; //allocate chopsticks
				else
					cArray = new bool[*numP + 1]; //allocate chopsticks
				pArray = new Philosopher[*numP]; //allocate philosophers
				printf("Initializing %d philosophers that will eat %d meals. \n", *numP, *numM);
				break;
			}
			else flag = true;	
			printf("Incorrect User Input. Please Enter a Positive Integer.\n\n");
		}
		for(int i = 0; i < *numP; i++)
		{
			cArray[i] = true; //all chopsticks are available
			pArray[i].id = i; //set Philosopher ID
			
			pArray[i].state = 0; //Everyone is entering

		}
		// Loop through each Philosopher and start a thread
		for(int i = 0; i < *numP; i++)
		{
	   		Thread *s = new Thread("Philosopher");
	   		s->Fork(PhilospherBusyWaitingAlgorithm, i);
		}
		
		currentThread->Finish();
    	}

	//Project 2 - Task 2
	//Edited by: Dustin Rowland
   	else if(ValidFlag == 4)
	{
		char *input = new char[100];
		bool flag;
		
	
		flag = true;
		while(flag)
		{
			printf("Enter the number of philosophers that will be dining: ");
			scanf("%s", input);
			// Validate user input
			if(!strcmp(validateInput(input),"Integer "))
			{
				*numPhils = atoi(input);
				if (*numPhils > 0)
					break;
			}
			printf("Incorrect User Input. Please Enter a Positive Integer.\n\n");

		}

		// Get number of meals
		flag = true;
		while(flag)
		{
			printf("Enter the number of meals the philosophers will eat: ");
			scanf("%s", input);
			// Validate user input
			if(!strcmp(validateInput(input),"Integer "))
			{
				*numMeals = atoi(input);
				if (*numMeals > 0)
					break;
			}
			printf("Incorrect User Input. Please Enter a Positive Integer.\n\n");
		}

		// Initialize groupState to be state 0
		groupState = 0;
		// Make array of states the size of the number of philosphers
		diningState = new int [*numPhils];
	
		// Initialize state of philosophers to be state 0
		for (int i=0; i<*numPhils; i++)
		{
			diningState[i] = 0;
		}


		// Make number of chopsticks the number of philosophers
		*numChopsticks = *numPhils;
		// Make array of semaphores the size of the number of philosphers
		if (*numPhils == 1)  	//if only 1 philosopher, make 2 chopsticks
		{
			*numChopsticks = 2;
			chopSticks = new Semaphore*[*numChopsticks];
		}
		else			
			chopSticks = new Semaphore*[*numChopsticks];

		// Initialize each semaphore in the array
		for (int i=0; i<*numChopsticks; i++)
		{
			chopSticks[i] = new Semaphore("Chopstick", 1);
		}

		// Loop through each Thread

		for(int i = 0; i < *numPhils; i++)
		{
	   		Thread *s = new Thread("Dining Philosophers Semaphores");
	   		s->Fork(DiningPhilSema, i);
		}

		currentThread->Finish();

	}
	//End editing by Dustin Rowland

	//Project 2-Task 3
	//Edited by: BRANDIN
    else if (ValidFlag == 5){
    	char test[100];
    	bool flag = true;
    	
    	while (flag){
    		printf("Insert number of people: ");
    		scanf("%s",test);
    		if(!strcmp(validateInput(test),"Integer "))
			{
				p = atoi(test);
				if (p>1) flag = false;
			}
			
			// Get number of meals
			printf("Insert number of messages to be sent: ");
			scanf("%s", test);
			// Validate user input
			if(!strcmp(validateInput(test),"Integer ") && flag == false)
			{
			 	m = atoi(test);
			 	if (m>0) flag = false;
			 	else flag = true;
			}	
			printf("Insert max number of messages a box can hold: ");
			scanf("%s",test);
			if (!strcmp(validateInput(test),"Integer ") && flag == false){
				maxMessage = atoi(test);
				if (maxMessage>0) break;
			}
			flag = true;
			printf("Incorrect User Input. Number of people must be greater than 1. Everything else must be greater than 0.\n\n");
		}
		//Can't use new to allocate a 2D array
    	boxArray = new MessageContent*[p];
    	for (int i=0; i<p;i++) boxArray[i] = new MessageContent[maxMessage];
    	
    	mailboxes = new Semaphore*[p];
    	MessageContent temp;
    	emptyMailBox = new bool[p];
    	
    	for (int i=0;i<p;i++){
    		Thread *k = new Thread("Mail People");
    		emptyMailBox[i] = true;
    		mailboxes[i] = new Semaphore("Mail slots",1);
    		for (int j=0;j<maxMessage;j++){
    			boxArray[i][j] = temp;
    		}
    		k->Fork(checkMailProcedure,i);
    	}
    	
    	currentThread->Finish();
    }
   else if (ValidFlag == 6){
    	char test[100];
    	bool flag = true;
    	
    	while (flag){
    		printf("Insert number of people: ");
    		scanf("%s",test);
    		if(!strcmp(validateInput(test),"Integer "))
			{
				p = atoi(test);
				flag = false;
			}
			
			// Get number of meals
			printf("Insert number of messages to be sent: ");
			scanf("%s", test);
			// Validate user input
			if(!strcmp(validateInput(test),"Integer ") && flag == false)
			{
			 	m = atoi(test);
			}	
			else flag= true;
			printf("Insert max number of messages a box can hold: ");
			scanf("%s",test);
			if (!strcmp(validateInput(test),"Integer ") && flag == false){
				maxMessage = atoi(test);
				break;
			}
			printf("Incorrect User Input. Please Enter a Positive Integer.\n\n");
		}
		//Can't use new to allocate a 2D array
    	boxArray = new MessageContent*[p];
    	for (int i=0; i<p;i++) boxArray[i] = new MessageContent[maxMessage];
    	
    	mailboxes = new Semaphore*[p];
    	emptyMailBox = new bool[p];
    	fullMailBox = new bool[p];
    	for (int i=0;i<p;i++){
    		Thread *k = new Thread("Mail People");
    		emptyMailBox[i] = true;
			fullMailBox[i] = false;
    		mailboxes[i] = new Semaphore("Mail slots",1);
    		for (int j=0;j<maxMessage;j++){
    			boxArray[i][j] = MessageContent();
    		}
    		k->Fork(checkMailProcedure,i);
    	}
    	
    	currentThread->Finish();
    }
    //Edited by: BRANDIN - End

}


