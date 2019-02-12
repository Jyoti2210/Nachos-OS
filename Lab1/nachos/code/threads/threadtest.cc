#include "main.h"
#include "kernel.h"


void
SimpleProcess(int which)
{
    cout<<"--------------------------------------------------------------\n";
    cout<<"Process running is: "<< kernel->currentProcess->getName()<< "\t Priority is: "<<kernel->currentProcess->getPriority()<<endl;
    cout<<"Thread running is: "<<kernel->currentProcess->currentThread->getName()<<endl;
    cout<<endl;
    cout<<"Main process finished here!"<<endl;
    kernel->currentProcess->Yield();   
}



void
printJoin(int which)
{
	if(!kernel->currentProcess->childProcessList->IsEmpty()){
		kernel->currentProcess->Join();	
	}
	
    cout<<"Process running is: "<< kernel->currentProcess->getName()<< "\t Priority is: "<<kernel->currentProcess->getPriority()<<endl;
    cout<<"Thread running is: "<<kernel->currentProcess->currentThread->getName()<<endl;
    cout<<endl;
   
}

void printProcess(int which){
	cout<<"-------------------------------------------------------------\n";
    cout<<"Process running is: "<< kernel->currentProcess->getName()<< "\t Priority is: "<<kernel->currentProcess->getPriority()<<endl;
    cout<<"Thread running is: "<<kernel->currentProcess->currentThread->getName()<<endl;
}



void
ThreadTest()
{
	
    Process *p1 = new Process("First Process",1,NULL);
    p1->Fork((VoidFunctionPtr) printProcess, (void *) 1);


    p1->createChildThread((VoidFunctionPtr) printProcess, (void *) 1, "Process1->Thread1");
    p1->createChildThread((VoidFunctionPtr) printProcess, (void *) 3, "Process1->Thread2");

    Process *p2 = new Process("Second Process",3,NULL);
    p2->Fork((VoidFunctionPtr) printProcess, (void *) 3);
    

	p2->createChildThread((VoidFunctionPtr) printProcess, (void *) 1 ,"Process2->Thread1");
    p2->createChildThread((VoidFunctionPtr) printProcess, (void *) 2, "Process2->Thread2");
    

    Process *p3 = new Process("Parent process",4,NULL);
    p3->Fork((VoidFunctionPtr) printJoin, (void *) 1);

    p3->createChildProcess((VoidFunctionPtr) printJoin,(void *) 3,"Child process1");
    p3->createChildProcess((VoidFunctionPtr) printJoin,(void *) 3,"Child process2");
    


    SimpleProcess(0);
}
