#include "sched.h"

#include<stdlib.h>
#include<stdio.h>

void f_step(REGISTER_FILE *regs, RETURN *r) {
	printf("step\n");

    r->cpu_time_taken = 1;
    r->state = PS_RUNNING;

    regs->r0--;

    if(regs->r0 == 0)
        r->state = PS_EXITED;
}

void f_init(REGISTER_FILE *regs, RETURN *r) {
	printf("init\n");
    
    r->cpu_time_taken = 1;
    r->state = PS_RUNNING;

    regs->r0 = 4;
}

void cpuHeavyStep(REGISTER_FILE *regs, RETURN *r) {
	printf("expensive step\n");

    r->cpu_time_taken = 5;
    r->state = PS_RUNNING;

    regs->r0--;

    if(regs->r0 == 0)
        r->state = PS_EXITED;
}


int fairTest() {
	SCHEDULER *s = new_scheduler(f_init);
    s->scheduler_algorithm = SA_FAIR;
    
    int i;
    for(i = 0; i < MAX_PROCESSES - 1; i++) {
        char name[4];
        snprintf(name, 4, "p%d", i + 1);

        printf("Fork and exec %s\n", name);
        PID pid = fork(s, 1);

        // Make every 3rd process use extra cpu.
        exec(s, pid, name, f_init, i % 3 == 0 ? cpuHeavyStep : f_step, -1);
        list_processes(s);
        printf("\n");
    }

    
    for(i = 0; i < (MAX_PROCESSES - 1) * 7; i++) {
        timer_interrupt(s);
        list_processes(s);
        printf("\n");
    }
}
