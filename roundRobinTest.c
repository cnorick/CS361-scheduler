#include "sched.h"

#include<stdlib.h>
#include<stdio.h>

void rr_step(REGISTER_FILE *regs, RETURN *r) {
	printf("step\n");

    r->cpu_time_taken = 1;
    r->state = PS_RUNNING;
}

void rr_init(REGISTER_FILE *regs, RETURN *r) {
	printf("init\n");
    
    r->cpu_time_taken = 1;
    r->state = PS_RUNNING;
}

int roundRobinTest() {
    printf("Testing Round Robin\n\n");

	SCHEDULER *s = new_scheduler(rr_init);
    s->scheduler_algorithm = SA_ROUND_ROBIN;
    
    int i;
    for(i = 0; i < MAX_PROCESSES - 1; i++) {
        char name[4];
        snprintf(name, 4, "p%d", i + 1);

        printf("Fork and exec %s\n", name);
        PID pid = fork(s, 1);
        exec(s, pid, name, rr_init, rr_step, 666);
        list_processes(s);
        printf("\n");
    }

    
    for(i = 0; i < (MAX_PROCESSES - 1) * 3; i++) {
        timer_interrupt(s);
        list_processes(s);
        printf("\n");
    }
}