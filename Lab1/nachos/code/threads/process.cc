
#include <iostream>
#include "copyright.h"
#include "switch.h"
#include "synch.h"
#include "sysdep.h"

static int id = 0;

using namespace std;

Scheduler1::Scheduler1()
{     
    readyList2 = new List<Thread *>;
    toBeDestroyed = NULL;
} 

Scheduler1::~Scheduler1()
{ 
    delete readyList2; 
} 

void Scheduler1::ReadyToRunThread (Thread *thread)
{
	
    ASSERT(kernel->interrupt->getLevel() == IntOff);
    thread->setStatus(READY);
    readyList2->Append(thread);
}


Thread *
Scheduler1::FindNextThreadToRun ()
{
	
    ASSERT(kernel->interrupt->getLevel() == IntOff);

    if (readyList2->IsEmpty()) {
       return NULL;
    } else {
        return readyList2->RemoveFront();
    }
    
}

void Scheduler1::RunThread(Thread* nextThread, bool finishing){
	
    Thread *oldThread = kernel->currentProcess->currentThread;
    
    ASSERT(kernel->interrupt->getLevel() == IntOff);

    if (finishing) {    
         ASSERT(toBeDestroyed == NULL);
     toBeDestroyed = oldThread;
    }
    
    if (oldThread->space != NULL) { 
        oldThread->SaveUserState();     
    oldThread->space->SaveState();
    }
    
    oldThread->CheckOverflow();
    kernel->currentProcess->currentThread = nextThread;
    nextThread->setStatus(RUNNING);
    
    SWITCH(oldThread, nextThread);

    ASSERT(kernel->interrupt->getLevel() == IntOff);

    CheckToBeDestroyed();       
    if (oldThread->space != NULL) {     
        oldThread->RestoreUserState();    
    oldThread->space->RestoreState();
    }
    
}

void
Scheduler1::CheckToBeDestroyed()
{
    if (toBeDestroyed != NULL) {
        delete toBeDestroyed;
	toBeDestroyed = NULL;
    }
}


int Process::getID(){
	return this->processID;
}

void Process::setID(int id){
	this->processID = id;
}

int Process::getPriority(){
	return this->priority;
}

void Process::setPriority(int priority){
	this->priority = priority;
}

string Process::getStatus(){
	return this->status;
}

void Process::setStatus(string status){
	this->status = status;
}

string Process::getName(){
	return this->pname;
}

void Process::setName(string name){
	this->pname = name;
}

Process::Process(char* name, int priority, Process* parentProcess){
	id = id+1;
	this->processID = id;
	this->priority = priority;
	this->pname = name;
	this->status = JUST_CREATED;
	this->parentProcess = parentProcess;
	this->scheduler = new Scheduler1();
	childProcessList = new List<Process*>();

	Thread *t = new Thread("Main Thread");
	this->currentThread = t;

	List<Thread*> *threadList = new List<Thread*>();
	threadList->Append(t);
}

Process::~Process(){
	delete scheduler;
	delete this->currentThread;
}

void
SimpleThread(int which)
{
    int num;
    
    for (num = 0; num < 5; num++) {
        printf("*** thread %d looped %d times\n", which, num);
        if(num == 4){
        	kernel->currentProcess->currentThread->Finish();
        }
        
    }
    kernel->currentProcess->currentThread->Yield();
}

void Process::Fork(VoidFunctionPtr func, void *arg) {
    Interrupt *interrupt = kernel->interrupt;
    Scheduler *scheduler = kernel->scheduler;   
    IntStatus oldLevel;

    this->currentThread->Fork(this,func,arg);
    oldLevel = interrupt->SetLevel(IntOff);

    this->currentThread = this->scheduler->FindNextThreadToRun();
   
    scheduler->ReadyToRunProcess(this);	// ReadyToRun assumes that interrupts 
					// are disabled!
   
    (void) interrupt->SetLevel(oldLevel);
} 


void Process::createChildThread(VoidFunctionPtr func, void *arg, char* tname){
	
	Thread *t = new Thread(tname);
	t->Fork(this,func,arg);
}

void Process::Yield ()
{	
    Process *nextProcess;
    IntStatus oldLevel = kernel->interrupt->SetLevel(IntOff);
   
    nextProcess = kernel->scheduler->FindNextProcessToRun();
    if (nextProcess != NULL) {
    	
		kernel->scheduler->ReadyToRunProcess(this);
		kernel->scheduler->RunProcess(nextProcess, FALSE);
    }
    (void) kernel->interrupt->SetLevel(oldLevel);
}


void Process::Sleep (bool finishing){
    Process *nextProcess;
    
    ASSERT(this == kernel->currentProcess);
    ASSERT(kernel->interrupt->getLevel() == IntOff);
    
    if(this->parentProcess && !this->parentProcess->childProcessList->IsEmpty()){
    	this->parentProcess->childProcessList->RemoveFront();
    	if(this->parentProcess->childProcessList->IsEmpty()){
    		kernel->scheduler->ReadyToRunProcess(this->parentProcess);
    	}
    }

    status = BLOCKED_PROCESS;
    while ((nextProcess = kernel->scheduler->FindNextProcessToRun()) == NULL)
	kernel->interrupt->Idle();	// no one to run, wait for an interrupt
    
    kernel->scheduler->RunProcess(nextProcess, finishing); 
}

void Process::Join(){
	cout<<"-----------------------------------------------------------"<<endl;
	cout<<"##############################################"<<endl;
    cout<<"JOIN function called"<<endl;
    cout<<"##############################################"<<endl;
	IntStatus oldLevel = kernel->interrupt->SetLevel(IntOff);
	this->Sleep(FALSE);
	(void) kernel->interrupt->SetLevel(oldLevel);

}

void Process::createChildProcess(VoidFunctionPtr func, void *arg, char* cpname){
	Process* child = new Process(cpname,this->priority,this);
	child->Fork(func,arg);
	this->childProcessList->Append(child);
}

void
Process::Finish ()
{
    (void) kernel->interrupt->SetLevel(IntOff);		
    ASSERT(this == kernel->currentProcess);

    
    Sleep(TRUE);				// invokes SWITCH
    // not reached
}

static void ProcessFinish()    { kernel->currentProcess->Finish(); }


void ProcessPrint(Process *t) { t->Print(); }


void
Process::SelfTest()
{
    DEBUG(dbgThread, "Entering Thread::SelfTest");

    Process *p = new Process("forked thread",1,NULL);

    p->Fork((VoidFunctionPtr) SimpleThread, (void *) 1);
    cout<<"yield is called in selftest"<<endl;
    kernel->currentProcess->Yield();
    //SimpleThread(0);
}



 



