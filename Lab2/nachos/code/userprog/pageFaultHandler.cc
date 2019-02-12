#include "main.h"
#include "pageFaultHandler.h"


//get the process(thread) details from the map to the page that is removed from the PageList
static TranslationEntry* getPhysPageID(int evictedPage){
	Thread *t  = Thread::getTID(kernel->pageProcessMap->find(evictedPage)->second);
	
	TranslationEntry *tempEntry = t->space->pageTable;

	for(int i = 0 ; i < t->space->numPages ; i++)
	{
		if(tempEntry[i].physicalPage == evictedPage){
			return &tempEntry[i];
		}
	}
	return NULL;
}

//handle page fault exception
void
PageFaultHandler::pgFaultHandler(int badAddress){
	
	int vpn = (int) badAddress / PageSize;	
	int newPhysPg = AddrSpace::bitmap->FindAndSet();

	TranslationEntry *pgEntry = NULL;

	if(newPhysPg != -1){
		pgEntry = kernel->currentThread->space->getPageTableEntry(vpn);
		int swapLocation = pgEntry->swapLocation;

		//read from the swapFile
		{
			char *buffer = new char[PageSize];
			kernel->swapFile->ReadAt(buffer, PageSize, swapLocation*PageSize);
			for(int k = 0, j = 0; j < PageSize ; k++, j++) {
		        kernel->machine->mainMemory[newPhysPg * PageSize + j] = buffer[k];
		    }	
		}

		//update the page table of the corresponding process
		{
			pgEntry->physicalPage = newPhysPg;
			pgEntry->valid = true;
			pgEntry->use = true;
		}

		//append the new page in the list and update the map with the corresponding process
		kernel->PageList->Append(newPhysPg);
		kernel->pageProcessMap->find(newPhysPg)->second = kernel->currentThread->getPID();

	} else {	
		//remove the front item from the fifo list
		
		int evictedPage = kernel->PageList->RemoveFront();
		
		//get the process(thread) of the evicted page
		pgEntry = getPhysPageID(evictedPage);
		

		if(pgEntry != NULL){
			//write the removed page from the main memory into swap file			
			{
				char *buffer = new char[PageSize];
				for(int k = 0, j = 0; j < PageSize ; k++, j++)
			    {
			        buffer[j] = kernel->machine->mainMemory[evictedPage * PageSize + k];
			    }
				kernel->swapFile->WriteAt(buffer, PageSize, pgEntry->swapLocation*PageSize);				
			}

			pgEntry->physicalPage = -1;
			pgEntry->valid = false;
			pgEntry->use = false;

			//read the data from the swap file into main memory 
			int swapLocation = kernel->currentThread->space->getPageTableEntry(vpn)->swapLocation;
			{
				char *buffer = new char[PageSize];
				kernel->swapFile->ReadAt(buffer, PageSize, swapLocation*PageSize);
				for(int k = 0, j = 0; j < PageSize ; k++, j++) {
			        kernel->machine->mainMemory[evictedPage * PageSize + j] = buffer[k];
			    }	
			}

			//update the page table entries of the new evicted page
			pgEntry = kernel->currentThread->space->getPageTableEntry(vpn);
			{
				pgEntry->physicalPage = evictedPage;
				pgEntry->valid = true;
				pgEntry->use = true;
			}
			
			// Updated from Map

			if(kernel->pageProcessMap->find(evictedPage) == kernel->pageProcessMap->end()){
				kernel->pageProcessMap->insert(pair <int,int> (evictedPage, kernel->currentThread->getPID()));	
			} else {
				kernel->pageProcessMap->find(evictedPage)->second = kernel->currentThread->getPID();
			}
			
			
			//Add to the  list
			kernel->PageList->Append(evictedPage);
		} else {
				cout<<"PageEntry Not Found in the Page Table.\n";
			}
	}

}
