// scheduler.cc 
//	Routines to choose the next thread to run, and to dispatch to
//	that thread.
//
// 	These routines assume that interrupts are already disabled.
//	If interrupts are disabled, we can assume mutual exclusion
//	(since we are on a uniprocessor).
//
// 	NOTE: We can't use Locks to provide mutual exclusion here, since
// 	if we needed to wait for a lock, and the lock was busy, we would 
//	end up calling FindNextToRun(), and that would put us in an 
//	infinite loop.
//
// 	Very simple implementation -- no priorities, straight FIFO.
//	Might need to be improved in later assignments.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "debug.h"
#include "main.h"
#include "kernel.h"
#include "scheduler.h"

//----------------------------------------------------------------------
// Scheduler::Scheduler
// 	Initialize the list of ready but not running threads.
//	Initially, no ready threads.
//----------------------------------------------------------------------
static int Compare(Process* p1, Process* p2){
    int priority1 = p1->getPriority();
    int priority2 = p2->getPriority();
    if(priority1 > priority2){
        return -1;
    }else if(priority1 == priority2){
        return 0;
    }else{
        return 1;
    }

}

Scheduler::Scheduler()
{ 
    readyList1 = new SortedList<Process *>(Compare); 
    toBeDestroyed = NULL;
} 

//----------------------------------------------------------------------
// Scheduler::~Scheduler
// 	De-allocate the list of ready threads.
//----------------------------------------------------------------------

Scheduler::~Scheduler()
{ 
    delete readyList1; 
} 

//----------------------------------------------------------------------
// Scheduler::ReadyToRun
// 	Mark a thread as ready, but not running.
//	Put it on the ready list, for later scheduling onto the CPU.
//
//	"thread" is the thread to be put on the ready list.
//----------------------------------------------------------------------

void
Scheduler::ReadyToRunProcess (Process *process)
{
    ASSERT(kernel->interrupt->getLevel() == IntOff);
    //DEBUG(dbgThread, "Putting thread on ready list: " << thread->getName());

    process->setStatus(READY_PROCESS);
    readyList1->Insert(process);
    
}




//----------------------------------------------------------------------
// Scheduler::FindNextToRun
// 	Return the next thread to be scheduled onto the CPU.
//	If there are no ready threads, return NULL.
// Side effect:
//	Thread is removed from the ready list.
//----------------------------------------------------------------------

Process *
Scheduler::FindNextProcessToRun ()
{
    
    ASSERT(kernel->interrupt->getLevel() == IntOff);


    if (readyList1->IsEmpty()) {
	   return NULL;
    } else {
    	return readyList1->RemoveFront();
    }
}


//----------------------------------------------------------------------
// Scheduler::Run
// 	Dispatch the CPU to nextThread.  Save the state of the old thread,
//	and load the state of the new thread, by calling the machine
//	dependent context switch routine, SWITCH.
//
//      Note: we assume the state of the previously running thread has
//	already been changed from running to blocked or ready (depending).
// Side effect:
//	The global variable kernel->currentThread becomes nextThread.
//
//	"nextThread" is the thread to be put into the CPU.
//	"finishing" is set if the current thread is to be deleted
//		once we're no longer running on its stack
//		(when the next thread starts running)
//----------------------------------------------------------------------

void
Scheduler::RunProcess (Process *nextProcess, bool finishing)
{    
    Process *oldProcess = kernel->currentProcess;
    Thread *oldThread = kernel->currentProcess->currentThread;


    if (finishing) {    // mark that we need to delete current thread
         // ASSERT(toBeDestroyed == NULL);
     toBeDestroyed = oldProcess;

    }


    Thread *nextThread = nextProcess->currentThread;
    
    ASSERT(kernel->interrupt->getLevel() == IntOff);

    kernel->currentProcess = nextProcess;
    nextProcess->setStatus(RUNNING_PROCESS);
    nextProcess->currentThread->setStatus(RUNNING);

    SWITCH(oldThread, nextThread);

    CheckToBeDestroyed();		// check if thread we were running
					// before this one has finished
					// and needs to be cleaned up

}



//----------------------------------------------------------------------
// Scheduler::CheckToBeDestroyed
// 	If the old thread gave up the processor because it was finishing,
// 	we need to delete its carcass.  Note we cannot delete the thread
// 	before now (for example, in Thread::Finish()), because up to this
// 	point, we were still running on the old thread's stack!
//----------------------------------------------------------------------

void
Scheduler::CheckToBeDestroyed()
{
    if (toBeDestroyed != NULL) {
        delete toBeDestroyed;
	toBeDestroyed = NULL;
    }

}
 
//----------------------------------------------------------------------
// Scheduler::Print
// 	Print the scheduler state -- in other words, the contents of
//	the ready list.  For debugging.
//----------------------------------------------------------------------
void
Scheduler::Print()
{
    cout << "Ready list contents:\n";
    readyList1->Apply(ProcessPrint);
    
}
