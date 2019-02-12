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

char receiver1[] = "test/prog10";
char receiver2[] = "test/prog11";
char message1[] = "hello9";
char message2[] = "hello9";
char *answerPtr1;
char *answerPtr2;
int bufferID1 = -1;
int bufferID2 = -1;
int result1 = -1;
int result2 = -1;
int
main()
{
     bufferID1 =  SendMessage(receiver1,message1,bufferID1);
     bufferID2 =  SendMessage(receiver2,message2,bufferID2);

     result1  =  WaitAnswer(result1,answerPtr1,bufferID1);
     result2  =  WaitAnswer(result2,answerPtr2,bufferID2);

     Exit(0);

  return 0;
}


