#include "sched.h"

// After three calls the process will ask to sleep.
void sl_step(REGISTER_FILE *regs, RETURN *r) {
	printf("step\n");

    r->cpu_time_taken = 1;
    r->state = PS_RUNNING;

    regs->r0++;

    if(regs->r0 == 3) {
        if(regs->r1 == 1) {
            r->state = PS_EXITED;
            return;
        }

        r->state = PS_SLEEPING;
        r->sleep_time = 10;
        regs->r1 = 1;
        regs->r0 = 0;
    }
}

void sl_init(REGISTER_FILE *regs, RETURN *r) {
	printf("init\n");
    
    r->cpu_time_taken = 1;
    r->state = PS_RUNNING;

    regs->r0 = 0;
    regs->r1 = 0;
}
int _sleepTest(SCHEDULER_ALGORITHM sa) {
	SCHEDULER *s = new_scheduler(sl_init);
    s->scheduler_algorithm = sa;
    
    int i;
    printf("Fork and exec p1\n");
    PID pid = fork(s, 1);
    exec(s, pid, "p1", sl_init, sl_step, -1);
    pid = fork(s, 1);
    exec(s, pid, "p2", sl_init, sl_step, -1);
    pid = fork(s, 1);
    exec(s, pid, "p3", sl_init, sl_step, -1);
    list_processes(s);
    printf("\n");

    
    for(i = 0; i < (MAX_PROCESSES - 1) * 10; i++) {
        timer_interrupt(s);
        list_processes(s);
        printf("\n");
    }
}
int sleepTest() {
    printf("Testing sleep for round robin\n");
    _sleepTest(SA_ROUND_ROBIN);
 //   
 //   printf("Testing sleep for fair\n");
 //   _sleepTest(SA_FAIR);
 //   
 //   printf("Testing sleep for fcfs\n");
 //   _sleepTest(SA_FCFS);
    
//    printf("Testing sleep for sjf\n");
//    _sleepTest(SA_SJF);
}
