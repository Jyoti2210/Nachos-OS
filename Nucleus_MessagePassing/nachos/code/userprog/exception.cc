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
#include <string>
#include <vector>

// #include "pageFaultExcHandler.h"
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



static map <int, Thread*> thread_map;

static void ForkTest1(int id)
{
	printf("ForkTest1 is called, its PID is %d\n", id);
	cout << "\nForkTest1";
	cout<<endl;
	for (int i = 0; i < 3; i++)
	{
		printf("ForkTest1 is in loop %d\n", i);
		for (int j = 0; j < 100; j++) 
			kernel->interrupt->OneTick();
	}
	cout<<endl;
	Exit_POS(id);
}

static void ForkTest2(int id)
{
	printf("ForkTest2 is called, its PID is %d\n", id);
	cout << "\nForkTest2";
	cout<<endl;
	for (int i = 0; i < 3; i++)
	{
		printf("ForkTest2 is in loop %d\n", i);
		for (int j = 0; j < 100; j++) 
			kernel->interrupt->OneTick();
	}
	cout<<endl;
	Exit_POS(id);
}

static void ForkTest3(int id)
{
	printf("ForkTest3 is called, its PID is %d\n", id);
	cout << "\nForkTest3";
	cout<<endl;
	for (int i = 0; i < 3; i++)
	{
		printf("ForkTest3 is in loop %d\n", i);
		for (int j = 0; j < 100; j++) 
			kernel->interrupt->OneTick();
	}
	cout<<endl;
	Exit_POS(id);
}

static
TranslationEntry* FetchPageEntry(int removedPage){
		// cout<<"*********  " <<kernel->page_table_map->find(removedPage)->second<<endl;
		Thread *temp_thread = Thread::get_thread_id(kernel->page_table_map->find(removedPage)->second);
		TranslationEntry *tempPTE = temp_thread->space->pageTable;
		// cout<<"getPageEntryByPhyPage"<<endl;
		for(int i = 0 ; i < temp_thread->space->numPages ; i++)
		{
			if(tempPTE[i].physicalPage == removedPage){
				return &tempPTE[i];
			}
		}
		// cout<<" eveted page "<<removedPage<<temp_thread->GetPid()<<endl;
		return NULL;			
}


int Join(){
    IntStatus oldLevel = kernel->interrupt->SetLevel(IntOff);
    kernel->currentThread->Sleep(false);
    kernel->interrupt->SetLevel(oldLevel);
    return 0;
}





static void EraseBuffer(int bufferID) {
	if(bufferID >=0)
		kernel->ClearKernelBufferQ(bufferID);
}



static void UpdateProgramCounter(){

	/* set previous programm counter (debugging only)*/
	kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

	/* set programm counter to next instruction (all Instructions are 4 byte wide)*/
	kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

	/* set next programm counter for brach execution */
	kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);

}

void
ExceptionHandler(ExceptionType which)
{
    int type = kernel->machine->ReadRegister(2);

    DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");
    // cout<<"exception type "<<which<<endl;
    switch (which) {
	    case SyscallException:
			switch(type) {
				case SC_Halt:
						DEBUG(dbgSys, "Shutdown, initiated by user program.\n");

						SysHalt();

						ASSERTNOTREACHED();
						break;

				case SC_Add:
						DEBUG(dbgSys, "Add " << kernel->machine->ReadRegister(4) << " + " << kernel->machine->ReadRegister(5) << "\n");
						
						/* Process SysAdd Systemcall*/
						int result;
						result = SysAdd(/* int op1 */(int)kernel->machine->ReadRegister(4),
								/* int op2 */(int)kernel->machine->ReadRegister(5));

						DEBUG(dbgSys, "Add returning with " << result << "\n");
						/* Prepare Result */
						kernel->machine->WriteRegister(2, (int)result);
						
						/* Modify return point */
						{
						  /* set previous programm counter (debugging only)*/
						  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

						  /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
						  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
						  
						  /* set next programm counter for brach execution */
						  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
						}

						return;
						
						ASSERTNOTREACHED();

				break;
				
				case SC_Fork_POS:
							{
								Thread *child_thread = new Thread("Child thread");
								int function_name_arg = kernel->machine->ReadRegister(4);

								if(function_name_arg == 1)
						    		child_thread->Fork((VoidFunctionPtr) ForkTest1, (void *) child_thread->GetPid());
						    	else if(function_name_arg == 2)
						    		child_thread->Fork((VoidFunctionPtr) ForkTest2, (void *) child_thread->GetPid());
						    	else if (function_name_arg == 3)
						    		child_thread->Fork((VoidFunctionPtr) ForkTest3, (void *) child_thread->GetPid());

						    	// kernel->machine->WriteRegister(2, 44);
						    	kernel->machine->WriteRegister(2, int(child_thread->GetPid()));
					    	 	/* Modify return point */
								{
								  /* set previous programm counter (debugging only)*/
								  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

								  /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
								  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
								  
								  /* set next programm counter for brach execution */
								  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
								}
								kernel->thread_list->Remove(child_thread);    	
						    	return;
						    	ASSERTNOTREACHED();
							}
				break;
				case SC_Wait_POS:
							{
								int child_pid = kernel->machine->ReadRegister(4);
						        thread_map[child_pid]= kernel->currentThread;
						        int join_status = Join();
						        kernel->machine->WriteRegister(2,join_status);

							    /* Modify return point */
								{
								  /* set previous programm counter (debugging only)*/
								  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

								  /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
								  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
								  
								  /* set next programm counter for brach execution */
								  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
								}
								return;
						    	ASSERTNOTREACHED();
							}
				break;
				
				case SC_Write:
							{
								int reg_add =kernel->machine->ReadRegister(4);
		            			int reg_size = kernel->machine->ReadRegister(5); 
								int val;
					            while(reg_size--){
					             	bool read_flag  = kernel->machine->ReadMem(reg_add, 1, &val);
					              	if (read_flag){
					            		printf("%c", val);
					            		reg_add++; 
					              }
					            }
								/* Modify return point */
								{
									/* set previous programm counter (debugging only)*/
									kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

									/* set programm counter to next instruction (all Instructions are 4 byte wide)*/
									kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

									/* set next programm counter for brach execution */
									kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
								}
								return;
								ASSERTNOTREACHED();
							}
				break;

				case SC_Exit:
						{

							// cout<<"exit"<<endl;

							int p_id = kernel->machine->ReadRegister(4);
							// cout<<"exit p_id =>"<<p_id<<endl;
							// checkWaitingProcess(arg);
							

							vector<int> v_keys;
							for(map<int,MessageBuffer*>::iterator it = kernel->currentThread->GetThreadMessageQueue()->begin(); it != kernel->currentThread->GetThreadMessageQueue()->end(); ++it) {
							  v_keys.push_back(it->first);
							  // cout << it->first << "\n";
							}
							// cout<<"exit"<<endl;
							for(int i =0; i<v_keys.size();i++){
								int buff = v_keys[i];

								if(kernel->currentThread->GetThreadMessageQueue()->count(buff) != 0 && kernel->currentThread->GetThreadMessageQueue()->find(buff)->second->getActiveFlag()) {
									
									// cout<<
									if (kernel->getMessageBufferQueue()->find(buff)->second->Front() != NULL){

										kernel->getMessageBufferQueue()->find(buff)->second->Front()->setActiveFlag(false);
									}

									if (kernel->getMessageBufferQueue()->find(buff)->second->Last() != NULL){
										kernel->getMessageBufferQueue()->find(buff)->second->Last()->setActiveFlag(false);
									}
									// kernel->getMessageBufferQueue()->find(buff)->second->Front()->setActiveFlag(false);

								
									EraseBuffer(buff);
								}

							}
							// cout<<"exit"<<endl;
							

							Exit_POS(kernel->currentThread->GetPid());

							    /* Modify return point */
							{
							  /* set previous programm counter (debugging only)*/
							  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

							  /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
							  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
							  
							  /* set next programm counter for brach execution */
							  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
							}

							return;
							ASSERTNOTREACHED();
						}
				break;


				case SC_SendMessage:
				{
					// char* message = "Hello from nProg1";
					cout<<"================================="<<endl;
					cout << "Entering Send Message..." << endl;
					IntStatus oldLevel = kernel->interrupt->SetLevel(IntOff);
					int receiverPtr = kernel->machine->ReadRegister(4);
					int messagePtr = kernel->machine->ReadRegister(5);
					int bufferID = kernel->machine->ReadRegister(6);
					//cout << "In SC_SendMessage 2" << endl;
					char* receiverName = kernel->machine->fetchInputFromPointer(receiverPtr);

					Thread *receiverProcess = Thread::GetThreadByName(receiverName);
					cout<<"Sending message from (Sender): " << kernel->currentThread->GetName() << endl;
					cout<<"Sending message to (Receiver): " << receiverName << endl;


					// cout << "msg  " <<message<< endl;
					// string msg = message;
					// cout << "msg  thread" <<new_msg<< endl;
					// cout << "Receiver Process====" << receiverProcess << endl;
					// cout<<"Buffer ID"<<bufferID<<endl;
					char* message;
					// cout << "msg  3" <<message<< endl;
					if(receiverProcess != NULL) {
						// cout << "msg  1" <<message<< endl;
						// cout << "in if" << endl;
						if(kernel->currentThread->GetMessageCount() <= kernel->getMessageLimit()) {



							if(bufferID  == -1) {

								MessageBuffer *newBuffer = new MessageBuffer(kernel->currentThread, receiverProcess);
								// cout << "msg  2" <<message<< endl;
								// cout<<"Buffer ID of new buffer"<<newBuffer->getBufferID()<<endl;
								if(newBuffer->getBufferID() == -1) {
									// cout<<"Message Buffer Exhausted, cannot proceed with communication. "<<endl;
									kernel->machine->WriteRegister(2, newBuffer->getBufferID());
									UpdateProgramCounter();
									kernel->interrupt->SetLevel(oldLevel);
									break;
								}
								message = kernel->machine->fetchInputFromPointer(messagePtr);
								// cout<<"Message sent: " << message <<endl;

								// cout<<"send msg"<<message<<endl;

								newBuffer->getMessages()->Append(message);
								// cout<<"list "<< newBuffer->getMessages()->Front()<<endl;
								// List<MessageBuffer> kernelBufferList = new ArrayList<MessageBuffer>();
								List<MessageBuffer*> *kernelBufferList = new List<MessageBuffer*>;
								kernelBufferList->Append(newBuffer);
								// cout<<"addr of bufferID"<<newBuffer<<endl;
								kernel->getMessageBufferQueue()->insert(pair <int, List<MessageBuffer*>* > (newBuffer->getBufferID(), kernelBufferList));
								receiverProcess->GetThreadMessageQueue()->insert(pair <int, MessageBuffer* > (newBuffer->getBufferID(), newBuffer));
								receiverProcess->GetThreadMessageQueue()->find(0);
								// cout << receiverProcess->GetName() << " PID" << endl;
								kernel->machine->WriteRegister(2, newBuffer->getBufferID());
							}
							else{

									// cout<<"else"<<endl;
									MessageBuffer *mBuffer = kernel->ValidateReceiver(bufferID,  receiverProcess);
									message = kernel->machine->fetchInputFromPointer(messagePtr);
									// cout<<"else 1"<<endl;
									if(mBuffer != NULL) {
										// cout<<"else2 "<<endl;

										mBuffer->getMessages()->Append(message);

										// cout << receiverProcess->GetName() << " PID" << endl;

										receiverProcess->GetThreadMessageQueue()->find(0);
											// cout<<"else 4"<< "" <<endl;
										//if (receiverProcess->GetThreadMessageQueue()->find(bufferID) != receiverProcess->GetThreadMessageQueue()->end() ){

										//	receiverProcess->GetThreadMessageQueue()->find(bufferID)->second = mBuffer;
										//}
										//else{
										//	cout<<"else 4"<<endl;
										receiverProcess->GetThreadMessageQueue()->insert(pair <int, MessageBuffer*> (bufferID	, mBuffer));

										//}
										// cout<<"else3"<<endl;
										//return value to user program
										kernel->machine->WriteRegister(2, bufferID);
									}
									else {
										//Pass some error message
										kernel->machine->WriteRegister(2, -1);
									}
							}
							cout<<"Message sent: " << message <<endl;
							cout<<"================================="<<endl;

						}
						else{
							
							MessageBuffer *mBuffer = kernel->ValidateReceiver(bufferID,receiverProcess);
								if(mBuffer != NULL) {
									mBuffer->getMessages()->Append("Temp_Message");
									receiverProcess->GetThreadMessageQueue()->insert(pair <int, MessageBuffer*> (bufferID	, mBuffer));

									//return value to user program
									kernel->machine->WriteRegister(2, bufferID);
								}
								
								// System.out.println("Maximum Messages Limit reached by process: " + JNachos.getCurrentProcess().getName());
								// System.out.println("Dummy Response sent to the process: " + receiverName);
						}


					}
					else{
						//no need to send dummy
						// cout << "SC_SendMessage EraseBuffer" << endl;
						EraseBuffer(bufferID);
					}

					UpdateProgramCounter();
					// cout<<"SC_SendMessage exit"<<endl;
					kernel->interrupt->SetLevel(oldLevel);
					return;	

				}
				break;



				case SC_WaitMessage:
				{
					cout<<"================================="<<endl;
					cout << "Entering SC_WaitMessage" << endl;
					IntStatus oldLevel = kernel->interrupt->SetLevel(IntOff);
					int  senderPtr = kernel->machine->ReadRegister(4);
					char* sender  = kernel->machine->fetchInputFromPointer(senderPtr);
					int  messagePtr = kernel->machine->ReadRegister(5);
					int  bufferID = kernel->machine->ReadRegister(6);

					while(true) {
							if(Thread::GetThreadByName(sender) != NULL) {
								cout<<"Received message from (Sender): "<<sender<<endl;
								cout<<"Receiver of the message (Receiver): "<<kernel->currentThread->getName()<<endl;

								MessageBuffer *mbuffer = kernel->currentThread->FetchThreadBuffer(bufferID,sender);
								if(mbuffer != NULL && !mbuffer->getMessages()->IsEmpty()) {

									char* message = mbuffer->getMessages()->RemoveFront();
									// cout<<"SC_WaitMessage message   "<<message<<endl;
									cout<<"Message recieved: "<<message<<endl;
									kernel->machine->writeMessageInMemory(message,messagePtr);
									kernel->machine->WriteRegister(2,mbuffer->getBufferID());
									break;
								}
								else {
									cout<<"No message in message buffer"<<endl;
									kernel->currentThread->Yield();
									// break;
								}
							}
							else 
							{
								MessageBuffer *mbuffer = kernel->currentThread->FetchThreadBuffer(bufferID,sender);
								if(mbuffer != NULL && !mbuffer->getMessages()->IsEmpty()) {
									char* message = mbuffer->getMessages()->RemoveFront();
									kernel->machine->writeMessageInMemory(message,messagePtr);
									kernel->machine->WriteRegister(2,mbuffer->getBufferID());
								}
								else 
								{
									//send dummy message
									EraseBuffer(bufferID);
									kernel->machine->writeMessageInMemory("Temp_Message",messagePtr);
									kernel->machine->WriteRegister(2,-1);
								}
								break;
							}
						}
					cout<<"================================="<<endl;
					UpdateProgramCounter();
					kernel->interrupt->SetLevel(oldLevel);
					return;	
				}
				// (kernel->currentThread->GetThreadMessageQueue()->find(bufferID))->second
				
				break;
				case SC_SendAnswer: {
					cout<<"================================="<<endl;
					cout << "Entering SC_SendAnswer" << endl;
					IntStatus oldLevel = kernel->interrupt->SetLevel(IntOff);



					// char* answer = "Answer from nProg2";
					int resultStatus = kernel->machine->ReadRegister(4);
					int answerPtr = kernel->machine->ReadRegister(5);
					char* answer = kernel->machine->fetchInputFromPointer(answerPtr);
					int bufferID = kernel->machine->ReadRegister(6);
					// cout<<"Answer 1"<<answer<<endl;
					MessageBuffer *mBuffer = kernel->currentThread->GetThreadMessageQueue()->find(bufferID)->second;
					if(mBuffer != NULL) {
						// cout<<"Answer 2"<<answer<<endl;
						// cout<<"buffer != null"<<endl;
						Thread* receiver = mBuffer->getSender();
						cout<<"Sending answer from (Sender): " << kernel->currentThread->GetName()<<endl;
						cout<<"Sending answer to (Receiver): " << receiver->GetName()<<endl;
						cout<<"Answer sent: " << answer << endl;
						if(receiver != NULL && Thread::IsProcessExist(receiver->GetPid())){
							// cout<<"Answer 3"<<answer<<endl;
							
							// cout<<"tetettetsjk"<<receiver->GetName()<<endl;
							if((receiver->GetThreadMessageQueue()->find(bufferID))->second == NULL){
								// cout<<"IF"<<endl;
								MessageBuffer *newBuffer = new MessageBuffer(kernel->currentThread, receiver, bufferID);
								// cout<<"Answer 4"<<answer<<endl;
								newBuffer->getMessages()->Append(answer);
								// cout<<"IF endl"<<endl;
								// cout<<"bufferID"<<newBuffer<<endl;
								// cout<<"addr of bufferID"<<newBuffer<<endl;
								// cout<<"find obj"<<kernel->getMessageBufferQueue()->find(bufferID)->second->NumInList()<<endl;
								
								kernel->getMessageBufferQueue()->find(bufferID)->second->Append(newBuffer);


								// List<MessageBuffer*> *kernelBufferList = (kernel->getMessageBufferQueue()->find(bufferID)->second)->Append(newBuffer);
								// cout<<"test"<<endl;

								// MessageBuffer *tempBuffer = new MessageBuffer(kernel->currentThread, receiver);
								// kernelBufferList->Append(newBuffer);
								// cout<<"append  end"<<endl;
								receiver->GetThreadMessageQueue()->insert(pair <int, MessageBuffer*> (bufferID, newBuffer));
							} 
							else 
							{
								// cout<<"else"<<endl;
								// cout<<"Answer 5"<<answer<<endl;
								receiver->GetThreadMessageQueue()->find(bufferID)->second->getMessages()->Append(answer);
							}



							kernel->machine->WriteRegister(2, kernel->currentThread->GetPid());
							// cout<<"\n Answer sent to process " << receiver->getName() << " with message: " << answer << "  by process " << kernel->currentThread->getName();
						} 

						else {
							EraseBuffer(bufferID);
							kernel->machine->WriteRegister(2,-1);
							// cout<<"\n Receiver either null or dead";
						}
					}
					cout<<"================================="<<endl;

					UpdateProgramCounter();


					kernel->interrupt->SetLevel(oldLevel);
      				return;	
				}
				break;

				case SC_WaitAnswer:{
					cout<<"================================="<<endl;
					cout << "Entering SC_WaitAnswer..." << endl;
					IntStatus oldLevel = kernel->interrupt->SetLevel(IntOff);


					int resultStatus = kernel->machine->ReadRegister(4);
					int answerPtr = kernel->machine->ReadRegister(5);
					int bufferID = kernel->machine->ReadRegister(6);
					char* sender = NULL;
					
					// cout<<"Sending message to (Receiver): " << receiverName << endl;
					// cout << "In SC_WaitAnswer 2" << endl;
					



					if((kernel->currentThread->GetThreadMessageQueue()->find(bufferID))->second != NULL){
						// cout << "In SC_WaitAnswer if" << endl;
						sender = (kernel->currentThread->GetThreadMessageQueue()->find(bufferID))->second->getSender()->GetName();

					} else {
						// cout << kernel->getMessageBufferQueue()->find(bufferID)->second << endl;
						sender = kernel->getMessageBufferQueue()->find(bufferID)->second->Front()->getReceiver()->GetName();
						// cout<<"sender of SC_WaitAnswer"<<sender<<endl;
						// cout << "In SC_WaitAnswer while if" << endl;
					}	

					while(true){

						if(Thread::GetThreadByName(sender) != NULL){
							
							MessageBuffer *mBuffer = kernel->currentThread->FetchThreadBuffer(bufferID, sender);
							if(mBuffer != NULL && !mBuffer->getMessages()->IsEmpty()){
								cout<<"================================="<<endl;
								cout << "Entering SC_WaitAnswer..." << endl;
								char* message = mBuffer->getMessages()->RemoveFront();
								kernel->machine->writeMessageInMemory(message, answerPtr);
								kernel->machine->WriteRegister(2, kernel->currentThread->GetPid());
								cout<<"Received Answer from (Sender): " << sender << endl;
								cout<<"Received Answer (Receiver): " << kernel->currentThread->GetName() << endl;
								cout<<"Answer Received: "<<message<<endl;
								cout<<"================================="<<endl;
								// cout<<"Answer Received by process " << kernel->currentThread->GetName() << " from process " << sender << ":" <<message;
								break;
							} 
							else {

								// cout << "SC_WaitAnswer+++++++++++++++++++++++++" << sender << endl;
								cout<<"Waiting for Answer from (Sender): " << sender << endl;
								cout<<"Waiting for Answer (Receiver): " << kernel->currentThread->GetName() << endl;
								// cout<<"Answer Not Received "<<endl;
								cout<<"================================="<<endl;
								kernel->currentThread->Yield();
								// break;
							}
						} 
						else {
							cout<<"================================="<<endl;
							cout << "Entering SC_WaitAnswer..." << endl;


							// cout << "In SC_WaitAnswer while else process is NULL" << endl;
							MessageBuffer *mBuffer = kernel->currentThread->FetchThreadBuffer(bufferID, sender);
							// cout<<"check"<<endl;
							if(mBuffer != NULL && !mBuffer->getMessages()->IsEmpty()){
								// cout<<"check1"<<endl;
								char* message = mBuffer->getMessages()->RemoveFront();
								cout<<"Received Answer from (Sender): " << sender << endl;
								cout<<"Received Answer (Receiver): " << kernel->currentThread->GetName() << endl;
								cout<<"Answer Received: "<<message<<endl;
								kernel->machine->writeMessageInMemory(message, answerPtr);
								kernel->machine->WriteRegister(2, kernel->currentThread->GetPid());
								// cout<<"\n Answer receieved by process " << kernel->currentThread->getName() << " from process " << sender << ":" << message;
							}
							else {
								cout<<"No Message Received"<<endl;
								// cout<<"check1 else"<<endl;
								EraseBuffer(bufferID);
								// cout<<"check1 else"<<endl;
								kernel->machine->writeMessageInMemory("Dummy Message", 0);
								// cout<<"check1 else"<<endl;
								kernel->machine->WriteRegister(2,0);
								// cout<<"check1 else"<<endl;
								// cout<<"\n Dummy response sent to: " << kernel->currentThread->getName() << " , process "<<sender << " exited with no response";
							}
							cout<<"================================="<<endl;
							break;
						}
					}
					// cout<<"while exit"<<endl;
					UpdateProgramCounter();
					// cout<<"while exit"<<endl;
					kernel->interrupt->SetLevel(oldLevel);
					// cout<<"while exit"<<endl;
      				return;
				}
				break;


			    default:
				cerr << "Unexpected system call " << type << "\n";
				break;
		      }
	      break;
	    
	    case PageFaultException:{
    	// cout<<"enter in PageFaultException"<<endl;
	    	int badAddress = kernel->machine->ReadRegister(BadVAddrReg);
	    	// cout<<"exit in PageFaultException"<<endl;	

	    	// ===============================================================
	    	int virtualPageNumber = (int) badAddress / PageSize;

			int physicalPageBitmap =  AddrSpace::bitmap_addspace->FindAndSet();

		    TranslationEntry *pageEntry = NULL;

		    if(physicalPageBitmap != -1) {
		        pageEntry = kernel->currentThread->space->getPageTableEntry(virtualPageNumber);

		        kernel->machine->ReadSwapSpace(pageEntry->swapLocation, physicalPageBitmap);

		        kernel->machine->UpdatePagetable(pageEntry, physicalPageBitmap);
		        // cout<<"list insert page nu "<<physicalPageBitmap<<endl;
		        kernel->list_fifo->Append(physicalPageBitmap);

		        kernel->page_table_map->find(physicalPageBitmap)->second = kernel->currentThread->GetPid();

		    } 
		    else {
		        int removedPage = kernel->list_fifo->RemoveFront();

		        pageEntry = FetchPageEntry(removedPage);

		        if(pageEntry != NULL){
		        	// cout<<"removed page"<<"  -> "<<removedPage<<"  , "<<"pid "<<kernel->currentThread->GetPid()<<endl;
		        	// cout<<"page fallt run"<<endl;
					kernel->machine->WriteIntoSwapSpace(pageEntry->swapLocation, removedPage);
					pageEntry->physicalPage = -1;
					pageEntry->valid = false;
					pageEntry->use = false;
					kernel->machine->ReadSwapSpace(kernel->currentThread->space->getPageTableEntry(virtualPageNumber)->swapLocation,removedPage);
					kernel->machine->UpdatePagetable(kernel->currentThread->space->getPageTableEntry(virtualPageNumber),removedPage);
					
					// Updated from Map
					if (kernel->page_table_map->find(removedPage) ==  kernel->page_table_map->end()){

						kernel->page_table_map->insert(pair <int,int> (removedPage, kernel->currentThread->GetPid()));
					}
					else{
						kernel->page_table_map->find(removedPage)->second = kernel->currentThread->GetPid();	
					}
					//Add to the  list
					// cout<<"list insert listfifo 2 "<<removedPage<<endl;
					kernel->list_fifo->Append(removedPage);
				}
				else{
					cout<<"PageEntry Not Found in the Page Table."<<endl;
				}
		    }
	    	// ==================================================================

	    }
	    return;
	    break;
	    default:
	    		cerr << "Unexpected user mode exception" << (int)which << "\n";
	    break;
    }
    ASSERTNOTREACHED();
}


void Exit_POS(int id)
{
	// cout<<"exit pos "<<id<<endl;

	ListIterator<Thread*> *iterator_b = new ListIterator<Thread*>(kernel->thread_list);
	// cout<<"numbs "<<kernel->thread_list->NumInList();
	for (; !iterator_b->IsDone(); iterator_b->Next()){
       
            
	    if (iterator_b->Item()->GetPid() == id)
	    {
	    	// cout<<"exit pos list"<<endl;
	        kernel->thread_list->Remove(iterator_b->Item());
	        // cout<<"exit pos list exit "<<endl;
	        break;

	    }
    
    } 
    // cout<<"numbs "<<kernel->thread_list->NumInList();


	// cout<<"exit pos "<<endl;



	map <int, Thread*> :: iterator itr;
	itr = thread_map.find(kernel->currentThread->GetPid());
	// cout<<"exit pos 1"<<endl;
	if(itr != thread_map.end()){
		IntStatus oldLevel = kernel->interrupt->SetLevel(IntOff);
		kernel->scheduler->ReadyToRun(itr->second);
		kernel->interrupt->SetLevel(oldLevel);
	}
		// cout<<"exit pos "<<endl;

	// cout<<"finish"<<endl;
	// cout<<"exit pos 2"<<endl;
	kernel->currentThread->Finish();
	// cout<<"exit pos 3"<<endl;
	// cout<<"finish"<<endl;

}



