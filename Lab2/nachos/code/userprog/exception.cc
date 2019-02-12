// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "main.h"
#include "syscall.h"
#include "ksyscall.h"
#include "pageFaultHandler.h"
#include <map>
//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// If you are handling a system call, don't forget to increment the pc
// before returning. (Or else you'll loop making the same system call forever!)
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	is in machine.h.
//----------------------------------------------------------------------
static map <int, Thread*> tmap;


//Sleep the current thread
int Join(){
	IntStatus oldLevel = kernel->interrupt->SetLevel(IntOff);
	kernel->currentThread->Sleep(FALSE);
	kernel->interrupt->SetLevel(oldLevel);
	return 0;
}

//update the PC to execute the next instruction
void updatePC(){
	  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));
	  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
	  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
}

static void ForkTest1(int id)
{
	cout<<endl;
	cout<<endl;
	printf("ForkTest1 is called, its PID is %d\n", id);
	for (int i = 0; i < 3; i++)
	{
		printf("ForkTest1 is in loop %d\n", i);
		for (int j = 0; j < 100; j++) 
			kernel->interrupt->OneTick();
	}
	Exit_POS(id);
}

static void ForkTest2(int id)
{
	cout<<endl;
	cout<<endl;
	printf("ForkTest2 is called, its PID is %d\n", id);
	for (int i = 0; i < 3; i++)
	{
		printf("ForkTest2 is in loop %d\n", i);
		for (int j = 0; j < 100; j++) 
			kernel->interrupt->OneTick();
	}
	Exit_POS(id);
}

static void ForkTest3(int id)
{
	cout<<endl;
	cout<<endl;
	printf("ForkTest3 is called, its PID is %d\n", id);
	for (int i = 0; i < 3; i++)
	{
		printf("ForkTest3 is in loop %d\n", i);
		for (int j = 0; j < 100; j++) 
			kernel->interrupt->OneTick();
	}
	Exit_POS(id);
}

void
ExceptionHandler(ExceptionType which)
{
    int type = kernel->machine->ReadRegister(2);

    DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");

    switch (which) {
    	case SyscallException:
      		switch(type) {
      			case SC_Halt:{
					DEBUG(dbgSys, "Shutdown, initiated by user program.\n");

					SysHalt();

					ASSERTNOTREACHED();
				}
				break;

				case SC_Add:{
					DEBUG(dbgSys, "Add " << kernel->machine->ReadRegister(4) << " + " << kernel->machine->ReadRegister(5) << "\n");
					
					/* Process SysAdd Systemcall*/
					int result;
					result = SysAdd(/* int op1 */(int)kernel->machine->ReadRegister(4),
							/* int op2 */(int)kernel->machine->ReadRegister(5));

					DEBUG(dbgSys, "Add returning with " << result << "\n");
					/* Prepare Result */
					kernel->machine->WriteRegister(2, (int)result);
					
					updatePC();

					return;
					
					ASSERTNOTREACHED();
				}
				
				break;

				// Implemented Fork System Call
				case SC_FORK_POS:{
					//setting interrupt off
					IntStatus oldLevel = kernel->interrupt->SetLevel(IntOff);
					int val = (int)kernel->machine->ReadRegister(4);
					//Create a new thread and called the corresponding ForkTest() while forking it
					Thread *t = new Thread("forked new thread");
					int pid = t->getPID();


					if(val == 1){
						t->Fork((VoidFunctionPtr) ForkTest1, (void *) pid); 
					}else if(val == 2){
						t->Fork((VoidFunctionPtr) ForkTest2, (void *) pid); 
					}else{
						t->Fork((VoidFunctionPtr) ForkTest3, (void *) pid); 
					}

					//Write the thread id into register 2
					kernel->machine->WriteRegister(2,pid);

					updatePC();
					kernel->threadList->Remove(t);
					//setting interrupt on
					kernel->interrupt->SetLevel(oldLevel);
					return;
					ASSERTNOTREACHED();
	
				}
				break;

				//Implementing Wait System Call
				case SC_WAIT_POS:{
					IntStatus oldLevel = kernel->interrupt->SetLevel(IntOff);
					int id = kernel->machine->ReadRegister(4);
					tmap[id] = kernel->currentThread;
					int status = Join();
					kernel->machine->WriteRegister(2,status);

					updatePC();
					kernel->interrupt->SetLevel(oldLevel);
					return;
					ASSERTNOTREACHED();
				}
				break;

				//Implementing Write System Call
				case SC_Write:{
					IntStatus oldLevel = kernel->interrupt->SetLevel(IntOff);
					cout<<endl;					
					cout<<endl;
					int virtualAddress = kernel->machine->ReadRegister(4);
					int val;

					int data;
					for ( int i = kernel->machine->ReadRegister(4); i < kernel->machine->ReadRegister(4) + kernel->machine->ReadRegister(5); i++)
					{
						kernel->machine->ReadMem(i, 1, &data);
						printf("%c", data);	
					}

					updatePC();
					kernel->interrupt->SetLevel(oldLevel);
					return;
				    ASSERTNOTREACHED();
					
				}
				break;

				//IMplementing Exit System Call
				case SC_Exit:{
					IntStatus oldLevel = kernel->interrupt->SetLevel(IntOff);
					//Call the Exit_POS() which will finish the current thread and put the corresponding thread from thread_map into ready queue

					Exit_POS(kernel->currentThread->getPID());
					
					updatePC();
					kernel->interrupt->SetLevel(oldLevel);
					return;
					ASSERTNOTREACHED();
				}
				break;

      			default:
					cerr << "Unexpected system call " << type << "\n";
					break;
			}
      	break;

      	// exception handler to handle page fault exceptions
      	case PageFaultException:{
      		//BadVAddrReg -> register 39

      		IntStatus oldLevel = kernel->interrupt->SetLevel(IntOff);
      		int badVirtualAddress = kernel->machine->ReadRegister(39);
      		//handle pagefault
      		PageFaultHandler:: pgFaultHandler(badVirtualAddress);
      		kernel->interrupt->SetLevel(oldLevel);
      		return;
      	}
      	break;
    	default:
      		cerr << "Unexpected user mode exception" << (int)which << "\n";
      		break;
    }
    ASSERTNOTREACHED();
}


void Exit_POS(int id){
	
	int threadID = id;
	map <int, Thread*> :: iterator iter;
	iter = tmap.find(threadID);

	if(iter != tmap.end()){
		IntStatus oldLevel = kernel->interrupt->SetLevel(IntOff);
	    kernel->scheduler->ReadyToRun(iter->second);
	    kernel->interrupt->SetLevel(oldLevel);
	}

	
	printf("\nExit system call made by process %d\n", kernel->currentThread->getPID());

  	kernel->currentThread->Finish();
}