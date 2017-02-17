#include "sched.h"

#include<stdlib.h>
#include<stdio.h>

void initp(REGISTER_FILE *regs, RETURN *r) {
	printf("starting init\n");

    r->cpu_time_taken = 1;
    r->state = PS_RUNNING;
}

void p1I(REGISTER_FILE *regs, RETURN *r) {
	printf("starting p1\n");
    
    regs->r0 = 10;

    r->cpu_time_taken = 1;
    r->state = PS_RUNNING;
}

void p1S(REGISTER_FILE *regs, RETURN *r) {
	printf("stepping p1\n");

    if(regs->r0-- == 0)
        r->state = PS_EXITED;
    else
        r->state = PS_RUNNING;

    r->cpu_time_taken = 1;
}


void p2I(REGISTER_FILE *regs, RETURN *r) {
	printf("starting p2\n");
    
    regs->r0 = 4;

    r->cpu_time_taken = 1;
    r->state = PS_RUNNING;
}

void p2S(REGISTER_FILE *regs, RETURN *r) {
	printf("stepping p2\n");

    if(regs->r0-- == 0)
        r->state = PS_EXITED;
    else
        r->state = PS_RUNNING;

    r->cpu_time_taken = 1;
}


int exitTest() {
    printf("Making scheduler\n");
	SCHEDULER *s = new_scheduler(initp);
    s->scheduler_algorithm = SA_ROUND_ROBIN;

	list_processes(s);
	printf("\n");
    
    int i;
    for(i = 0; i < MAX_PROCESSES - 1; i++) {
        char name[4];
        snprintf(name, 4, "p%d", i + 1);

        printf("Fork and exec %s\n", name);
        PID pid = fork(s, 1);
        if(i % 2 == 0)
            exec(s, pid, name, p2I, p2S, -1);
        else
            exec(s, pid, name, p1I, p1S, -1);
        list_processes(s);
        printf("\n");
    }

    for(i = 0; i < MAX_PROCESSES * 8; i++) {
        timer_interrupt(s);
        list_processes(s);
        printf("\n");
    }

    for(i = 0; i < MAX_PROCESSES - 1; i++) {
        char name[4];
        snprintf(name, 4, "p%d", i + 1);

        printf("Fork and exec %s\n", name);
        PID pid = fork(s, 1);
        if(i % 2 == 0)
            exec(s, pid, name, p2I, p2S, -1);
        else
            exec(s, pid, name, p1I, p1S, -1);
        list_processes(s);
        printf("\n");
    }
}
