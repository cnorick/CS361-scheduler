#include "sched.h"
#include <stdlib.h>

SCHEDULER *s;

PROCESS_CODE(mInit) {
    //mutex_create(s);
}

PROCESS_CODE(mStep) {
    //mutex_create(s);
}

void printMutexes(SCHEDULER *s) {
    int i;
    printf("Mutex State\n");
    for(i = 0; i < MAX_MUTEX; i++) {
        if(s->mutex_list[i] != -1)
                printf("%-5d %-8s\n", i, s->mutex_list[i] == 1 ? "locked" : "unlocked");
    }
    printf("\n");
}

int mutexTest() {
    printf("Making scheduler\n");
	SCHEDULER *s = new_scheduler(mInit);
    s->scheduler_algorithm = SA_ROUND_ROBIN;
    PID pid;
    int didLock;

    pid = fork(s, 1);
    exec(s, pid, "p1", mInit, mStep, -1);

    pid = fork(s, 1);
    exec(s, pid, "p2", mInit, mStep, -1);


    MUTEX m = mutex_create(s);
    printf("Created Mutex %d\n", m);
    printMutexes(s);

    didLock = mutex_lock(s, m);
    printf("Lock Mutex %d: %ssuccessful\n", m, didLock == 1 ? "" : "un");
    printMutexes(s);

    didLock = mutex_lock(s, m);
    printf("Lock Mutex %d: %ssuccessful\n", m, didLock == 1 ? "" : "un");
    printMutexes(s);

    mutex_unlock(s, m);
    printf("Unlock Mutex %d.\n", m);
    printMutexes(s);

    didLock = mutex_lock(s, m);
    printf("Lock Mutex %d: %ssuccessful\n", m, didLock == 1 ? "" : "un");
    printMutexes(s);

    
    while(1) {
        MUTEX m = mutex_create(s);
        if(m == -1) {
            printf("Unable to create mutex\n");
            break;
        }
        else
            printf("Successfully created mutex %d\n", m);
    }

    printMutexes(s);

    int i;
    for(i = 0; i < MAX_MUTEX; i++) {
        mutex_destroy(s, i);
        printMutexes(s);
    }
}
