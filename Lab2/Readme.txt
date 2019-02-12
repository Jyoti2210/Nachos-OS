##################
##   Objective  ##
##################

The main objective of this lab is broken down to following 4 tasks:
	1)	Implement following system calls:
		a.	FORK_POS
		b.	WAIT_POS
		c.	WRITE_POS
		d.	EXIT
	2)	Implement multiprogramming wherein the execution of multiple user programs invokes the “WRITE” system call.
	3)	Implement the idea of virtual memory and page replacement algorithm for running a program whose size is greater than the 		 size of the main memory.
	4)	Implement round robin scheduling algorithm.


###########################
## Steps to run the code ##
###########################

Navigate to nachos/code/build.linux/ and run the following commands:   
1) make clean   
2) make depend   
3) make   

#Task 1:  
./nachos -x ../test/prog1 ./nachos -x ../test/prog2 ./nachos -x ../test/prog3 
 
#Task 2: 
./nachos -x ../test/mprog1 -x ../test/mprog2 
 
#Task 3: 
./nachos -x ../test/matmult  or ./nachos -quantum 1000 -x ../test/matmult -x ../test/matmult 
 
#Task 4: 
(kindly keep the quantum size > 1000) 
./nachos -quantum 1000 -x ../test/mprog1 -x ../test/mprog2 
