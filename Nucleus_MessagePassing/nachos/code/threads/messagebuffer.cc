#include "kernel.h"
// #include "messagebuffer.h"

MessageBuffer::MessageBuffer(Thread* sender, Thread* receiver){
	this->sender = sender;
	this->receiver = receiver;
	this->bufferID = Kernel::getMessageBuffer()->FindAndSet();
	this->messages = new List<char*>();
	activeStatus = true;
}

MessageBuffer::MessageBuffer(Thread* sender, Thread* receiver, int bufferID){
	this->sender = sender;
	this->receiver = receiver;
	this->bufferID = bufferID;
	this->messages = new List<char*>();
	activeStatus = true;
}

MessageBuffer::~MessageBuffer() {

}

Thread*
MessageBuffer::getSender(){
	return this->sender;
}

Thread*
MessageBuffer::getReceiver(){
	return this->receiver;
}

int
MessageBuffer::getBufferID(){
	return this->bufferID;
}

bool
MessageBuffer::getActiveFlag(){
	return this->activeStatus;
}

void
MessageBuffer::setActiveFlag(bool flag){
	this->activeStatus = flag;
}


List<char*>* 
MessageBuffer::getMessages(){
	return this->messages;

}