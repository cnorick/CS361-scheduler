#include "sched.h"
#include <stdlib.h>

SCHEDULER *s;

PROCESS_CODE(sInit) {
}

PROCESS_CODE(sStep) {
}

void printSems(SCHEDULER *s) {
    int i;
    printf("Sem Num\n");
    for(i = 0; i < MAX_SEM; i++) {
        if(s->sem_list[i] != -1)
                printf("%-3d %-3d\n", i, s->sem_list[i]);
    }
    printf("\n");
}

int semaphoreTest() {
    printf("Making scheduler\n");
	SCHEDULER *s = new_scheduler(sInit);
    s->scheduler_algorithm = SA_ROUND_ROBIN;
    PID pid;
    int num;

    pid = fork(s, 1);
    exec(s, pid, "p1", sInit, sStep, -1);

    pid = fork(s, 1);
    exec(s, pid, "p2", sInit, sStep, -1);


    SEMAPHORE m = sem_create(s);
    printf("Created Sem %d\n", m);
    int i;
    for(i = 0; i < 10; i++)
        sem_up(s, m);
    printSems(s);

    while(1) {
        num = sem_down(s, m);
        if(num > 0)
            printf("Sem %d down from %d\n", m, num);
        else {
            printf("Out of resources\n");
            break;
        }
        printSems(s);
    }

    while(1) {
        SEMAPHORE m = sem_create(s);
        if(m == -1) {
            printf("Unable to create sem\n");
            break;
        }
        else
            printf("Successfully created sem %d\n", m);
    }

    printSems(s);

    for(i = 0; i < MAX_SEM; i++) {
        sem_destroy(s, i);
        printSems(s);
    }
}
