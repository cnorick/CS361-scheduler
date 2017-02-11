#include "sched.h"

#include<stdlib.h>
#include<stdio.h>

REGISTER_FILE initRegs = { .r0 = 42, .r1 = 666, .r2 = 9, .r3 = -44 };
const REGISTER_FILE p1Regs = { .r0 = 0, .r1 = 1, .r2 = 2, .r3 = 3 };


void init(REGISTER_FILE *regs, RETURN *r) {
	printf("starting init\n");

    regs->r0 = initRegs.r0;
    regs->r1 = initRegs.r1;
    regs->r2 = initRegs.r2;
    regs->r3 = initRegs.r3;

    r->cpu_time_taken = 1;
    r->state = PS_RUNNING;
}

void p1Init(REGISTER_FILE *regs, RETURN *r) {
	printf("starting p1\n");

    // Mess with the registers some.
    regs->r0 = p1Regs.r0;
    regs->r1 = p1Regs.r1;
    regs->r2 = p1Regs.r2;
    regs->r3 = p1Regs.r3;

    r->cpu_time_taken = 1;
    r->state = PS_RUNNING;
}

int p1Num = 0;
void p1Step(REGISTER_FILE *regs, RETURN *r) {
	printf("stepping p1\n");

    // Check that the registers are the same as they were in p1Init.
    if(regs->r0 != p1Regs.r0) {
        perror("ERROR: Registers don't match!");
        exit(1);
    }
    if(regs->r1 != p1Regs.r1) {

        perror("ERROR: Registers don't match!");
        exit(1);
    }
    if(regs->r2 != p1Regs.r2) {
        perror("ERROR: Registers don't match!");
        exit(1);
    }
    if(regs->r3 != p1Regs.r3) {
        perror("ERROR: Registers don't match!");
        exit(1);
    }

    // Eventually set state in RETURN as exit.
    
    r->cpu_time_taken = 1;
    r->state = PS_RUNNING;
}

void p2Init(REGISTER_FILE *regs, RETURN *r) {
	printf("starting p2\n");
    
    r->cpu_time_taken = 1;
    r->state = PS_RUNNING;
}

void p2Step(REGISTER_FILE *regs, RETURN *r) {
	printf("stepping p2\n");
    
    r->cpu_time_taken = 1;
    r->state = PS_RUNNING;
}

void p3Init(REGISTER_FILE *regs, RETURN *r) {
	printf("starting p3\n");
    
    r->cpu_time_taken = 1;
    r->state = PS_RUNNING;
}

void p3Step(REGISTER_FILE *regs, RETURN *r) {
	printf("stepping p3\n");
    
    r->cpu_time_taken = 1;
    r->state = PS_RUNNING;
}


int main() {
    roundRobinTest();
/*
    printf("Making scheduler\n");
	SCHEDULER *s = new_scheduler(init);
    s->scheduler_algorithm = SA_ROUND_ROBIN;

    printf("Calling timer_interrupt with no processes scheduled\n");
    timer_interrupt(s);
	list_processes(s);
	printf("\n");
    
	// fork init.
    printf("Fork and exec p1\n");
	PID pid = fork(s, 1);
    exec(s, pid, "p1", p1Init, p1Step, 666);
	list_processes(s);
	printf("\n");

    // Now fork p1.
    printf("Fork and exec p2\n");
    pid = fork(s, pid);
    exec(s, pid, "p2", p2Init, p2Step, 667);
	list_processes(s);
	printf("\n");

    // fork init again.
    printf("Fork and exec p3\n");
    pid = fork(s, 1);
    exec(s, pid, "p3", p3Init, p3Step, 668);
	list_processes(s);
	printf("\n");

    
    printf("Timer interrupt\n");
    timer_interrupt(s);
	list_processes(s);
	printf("\n");
    
    printf("Timer interrupt\n");
    timer_interrupt(s);
	list_processes(s);
	printf("\n");
    
    printf("Timer interrupt\n");
    timer_interrupt(s);
	list_processes(s);
	printf("\n");

*/

	return 0;
}
