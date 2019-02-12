/* halt.c
 *	Simple program to test whether running a user program works.
 *	
 *	Just do a "syscall" that shuts down the OS.
 *
 * 	NOTE: for some reason, user programs with global data structures 
 *	sometimes haven't worked in the Nachos environment.  So be careful
 *	out there!  One option is to allocate data structures as 
 * 	automatics within a procedure, but if you do this, you have to
 *	be careful to allocate a big enough stack to hold the automati
*/

#include "syscall.h"

char receiver[] = "test/prog4";
char message1[] = "hello3";
char message2[] = "hi3";
char *answerPtr;
int bufferID = -1;
int result1 = -1;
int result2 = -1;
int
main()
{
     bufferID =  SendMessage(receiver,message1,bufferID);
     bufferID =  SendMessage(receiver,message2,bufferID);

     result1  =  WaitAnswer(result1,answerPtr,bufferID);
     result2  =  WaitAnswer(result2,answerPtr,bufferID);

     Exit(0);

  return 0;
}