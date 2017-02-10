#include "sched.h"

#include<stdlib.h>
#include<stdio.h>

REGISTER_FILE initRegs = { .r0 = 42, .r1 = 666, .r2 = 9, .r3 = -44 };
REGISTER_FILE p1Regs = { .r0 = 0, .r1 = 1, .r2 = 2, .r3 = 3 };


void init(REGISTER_FILE *regs, RETURN *r) {
	printf("starting init\n");

    regs->r0 = initRegs.r0;
    regs->r1 = initRegs.r1;
    regs->r2 = initRegs.r2;
    regs->r3 = initRegs.r3;
}

void p1Init(REGISTER_FILE *regs, RETURN *r) {
	printf("starting p1\n");

    // Mess with the registers some.
    regs->r0 = p1Regs.r0;
    regs->r1 = p1Regs.r1;
    regs->r2 = p1Regs.r2;
    regs->r3 = p1Regs.r3;
}

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
}

void p2Init(REGISTER_FILE *regs, RETURN *r) {
	printf("starting p2\n");
}

void p2Step(REGISTER_FILE *regs, RETURN *r) {
	printf("stepping p2\n");
}

void p3Init(REGISTER_FILE *regs, RETURN *r) {
	printf("starting p3\n");
}

void p3Step(REGISTER_FILE *regs, RETURN *r) {
	printf("stepping p3\n");
}

void getInit() {

}


int main() {
	SCHEDULER *s = new_scheduler(init);
    
    REGISTER_FILE r = s->active_registers;
    printf("r0: %d, r1: %d, r2: %d, r3: %d\n", r.r0, r.r1, r.r2, r.r3);
    timer_interrupt(s);
    r = s->process_list[0].saved_registers;
    printf("r0: %d, r1: %d, r2: %d, r3: %d\n", r.r0, r.r1, r.r2, r.r3);

	// fork init.
	PID pid = fork(s, 1);
    exec(s, pid, "p1", p1Init, p1Step, 666);

    // Now fork p1.
    pid = fork(s, pid);
    exec(s, pid, "p2", p2Init, p2Step, 667);

    // fork init again.
    pid = fork(s, 1);
    exec(s, pid, "p3", p3Init, p3Step, 668);

    
    timer_interrupt(s);



	printf("\n");
	list_processes(s);


	return 0;
}
