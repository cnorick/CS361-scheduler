#include "sched.h"

#include<stdlib.h>
#include<stdio.h>

void init_code(REGISTER_FILE *regs, RETURN *r) {
    r->cpu_time_taken = 1;
    r->state = PS_RUNNING;
}
void rr_step(REGISTER_FILE *regs, RETURN *r) {
	printf("step\n");

    r->cpu_time_taken = 1;
    r->state = PS_RUNNING;

    regs->r0--;

    if(regs->r0 == 0)
        r->state = PS_EXITED;
}

void rr_init(REGISTER_FILE *regs, RETURN *r) {
	printf("init\n");
    
    r->cpu_time_taken = 1;
    r->state = PS_RUNNING;

    regs->r0 = 2;
}

int roundRobinTest() {
	SCHEDULER *s = new_scheduler(init_code);
    s->scheduler_algorithm = SA_ROUND_ROBIN;
    
    int i;
    for(i = 0; i < MAX_PROCESSES - 1; i++) {
        char name[4];
        snprintf(name, 4, "p%d", i + 1);

        printf("Fork and exec %s\n", name);
        PID pid = fork(s, 1);
        exec(s, pid, name, rr_init, rr_step, -1);
        list_processes(s);
        printf("\n");
    }

    
    for(i = 0; i < (MAX_PROCESSES - 1) * 4; i++) {
        timer_interrupt(s);
        list_processes(s);
        printf("\n");
    }
}
