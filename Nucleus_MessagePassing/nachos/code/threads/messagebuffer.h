// #include "kernel.h"
// #include "thread.h"
#include "list.h"
#include <vector>

class Thread;
// class Kernel;

class MessageBuffer {
	private:
		Thread* sender;
		Thread* receiver;
		int bufferID;
		bool activeStatus;
		List<char*> *messages;

	public:
		MessageBuffer(Thread* sender, Thread* receiver);
		MessageBuffer(Thread* sender, Thread* receiver, int bufferID);
		~MessageBuffer();

		Thread* getSender();
		Thread* getReceiver();
		int getBufferID();
		List<char*>* getMessages();
		bool getActiveFlag();
		void setActiveFlag(bool flag);

};