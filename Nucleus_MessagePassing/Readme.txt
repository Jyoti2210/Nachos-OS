##################
##   Objective  ##
##################

The main objective of this project is to implement multiprogramming and communication between different processes coordinated by the message buffering process within the system nucleus.

All the processes communicate using the following 4 system calls:
1. sendMessage(receiver, message, buffer) 
2. waitMessage(sender, message, buffer) 
3. sendAnswer(result, answer, buffer) 
4. waitAnswer(result, answer, buffer) 

###########################
## Steps to run the code ##
###########################
Navigate to nachos/code/build.linux and run the following commands:  
1) make clean
2) make depend 
3) make 

The various test cases can be checked using the following commands: 
./nachos -x ../test/prog1 -x ../test/prog2 
./nachos -x ../test/prog3 -x ../test/prog4 
./nachos -x ../test/prog5 -x ../test/prog6 
./nachos -x ../test/prog7 -x ../test/prog8 
./nachos -x ../test/prog9 -x ../test/prog10 -x ../test/prog11 