#include "sched.h"

#include<stdlib.h>
#include<stdio.h>



int main() {
    printf("\n\n===================Testing Round Robin=====================\n\n");
//    roundRobinTest();
    printf("\n\n===================Testing FCFS============================\n\n");
//    fcfsTest();
    printf("\n\n===================Testing Fair============================\n\n");
    fairTest();
    printf("\n\n===================Testing SJF============================\n\n");
//    sjfTest();
    printf("\n\n===================Testing Registers============================\n\n");
    registerTest();

	return 0;
}
