#include "sched.h"

#include<stdlib.h>
#include<stdio.h>

void sjf_step(REGISTER_FILE *regs, RETURN *r) {
	printf("step\n");

    r->cpu_time_taken = 1;
    r->state = PS_RUNNING;

    regs->r0--;

//    if(regs->r0 == 0)
        //r->state = PS_EXITED;
}

void sjf_init(REGISTER_FILE *regs, RETURN *r) {
	printf("init\n");
    
    r->cpu_time_taken = 1;
    r->state = PS_RUNNING;

    // r0 will contain the number of time until exit.
    regs->r0 = 2;
}

int sjfTest() {
    printf("Testing FCFS\n\n");

	SCHEDULER *s = new_scheduler(sjf_init);
    s->scheduler_algorithm = SA_SJF;
    
    int i;
    for(i = 0; i < MAX_PROCESSES - 1; i++) {
        char name[4];
        snprintf(name, 4, "p%d", i + 1);

        printf("Fork and exec %s\n", name);
        PID pid = fork(s, 1);
        exec(s, pid, name, sjf_init, sjf_step, MAX_PROCESSES - i);
        list_processes(s);
        printf("\n");
    }

    
    for(i = 0; i < 200; i++) {
        timer_interrupt(s);
        list_processes(s);
        printf("\n");
    }
}
