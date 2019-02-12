#include "copyright.h"
#include "utility.h"
#include "sysdep.h"
#include "machine.h"
#include "addrspace.h"
#include "thread.h"
#include "kernel.h"
#include <iostream>

using namespace std;

enum ProcessStatus { JUST_CREATED_PROCESS, RUNNING_PROCESS, READY_PROCESS, BLOCKED_PROCESS };

class Scheduler1{
	// The following class defines the scheduler/dispatcher abstraction -- 
	// the data structures and operations needed to keep track of which 
	// thread is running, and which threads are ready but not running.

	public:
		Scheduler1();		// Initialize list of ready threads 
		~Scheduler1();		// De-allocate ready list

		void ReadyToRunThread(Thread* thread);
						// Thread can be dispatched.
		Thread* FindNextThreadToRun();	// Dequeue first thread on the ready 
					// list, if any, and return thread.
		void RunThread(Thread* nextThread, bool finishing);
						// Cause nextThread to start running
		void CheckToBeDestroyed();// Check if thread that had been
						// running needs to be deleted
		void Print();		// Print contents of ready list

	// SelfTest for scheduler is implemented in class Thread

	private:
		List<Thread*> *readyList2;  // queue of threads that are ready to run,
					// but not running
		Thread *toBeDestroyed;	// finishing thread to be destroyed
					// by the next thread that runs
};



class Process {

	private:
		int processID;
		int priority;
		string status;
		string pname;

	public:

		Thread *currentThread;
		Scheduler1 *scheduler;
		
		int getID();
		void setID(int id);

		int getPriority();
		void setPriority(int p);

		string getStatus();
		void setStatus(string s);

		string getName();
		void setName(string s);

		
		Process(char* name, int p, Process* parentProcess);
		~Process();

	
		List<Process*> *childProcessList;
		Process* parentProcess;
		void Yield();
		void setStatus(ProcessStatus st) { status = st; }

		void Fork(VoidFunctionPtr func, void *arg); 
		void createChildProcess(VoidFunctionPtr func, void *arg, char* cpname);
		void SelfTest();
		void Print(){cout<<pname; cout<<" "; cout<<priority;}

		void Sleep(bool finishing);
		void ProcessFinish();
		void Finish();
		void Join();

		void createChildThread(VoidFunctionPtr func, void *arg, char* tname);
};

extern void ProcessPrint(Process *process);
