#include "sched.h"

#include<stdlib.h>
#include<stdio.h>



int main() {
    printf("\n\n===================Testing Round Robin=====================\n\n");
    roundRobinTest();
    printf("\n\n===================Testing FCFS============================\n\n");
    fcfsTest();
    printf("\n\n===================Testing Fair============================\n\n");
    fairTest();
    printf("\n\n===================Testing SJF============================\n\n");
    sjfTest();
    printf("\n\n===================Testing Registers============================\n\n");
    registerTest();
    printf("\n\n===================Testing Sleeping============================\n\n");
    sleepTest();
    printf("\n\n===================Testing Exiting and Adding More============================\n\n");
    exitTest();
    printf("\n\n===================Testing Marzzzz============================\n\n");
    marzTest();
    printf("\n\n===================Testing Mutex============================\n\n");
    mutexTest();
 

	return 0;
}
