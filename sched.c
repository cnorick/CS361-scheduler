#include "sched.h"
#include <stdlib.h>
#include <stdio.h>

//Simulate a timer interrupt from hardware. This should initiate
////the context switch procedure
//// - Context switch must save active process' state into the PROCESS structure
//// - Context switch must load the next process' state into the scheduler
void timer_interrupt(SCHEDULER *s) {
	
}

////Create a new scheduler
////This function needs to
//// - Create a new scheduler (on heap)
//// - Schedule process 1 (init)
//// - Set process 1 to current process
//// - Return the created scheduler
SCHEDULER *new_scheduler(PROCESS_CODE_PTR(initCode)) {
	// Create new scheduler.
	SCHEDULER *s = (SCHEDULER*) malloc(sizeof(SCHEDULER));

	// Create process 1.
	PROCESS* init = (PROCESS*) malloc(sizeof(PROCESS));
	init->name = "init";
	init->pid = 1;
	init->switched = 0;
	init->total_cpu_time = 0;
	init->switched_cpu_time = 0;
	init->job_time = -1;
	init->saved_registers = *((REGISTER_FILE*) malloc(sizeof(REGISTER_FILE)));
	init->state = PS_NONE;
	init->init = initCode;
	//init->step = 


	// Load init into the scheculer and set it as running.
	s->process_list[0] = *init;
	s->current = 0;
	//s->scheduler_algorithm = 
	s->active_registers = init->saved_registers;

	s->process_list[s->current].state = PS_RUNNING;
	s->process_list[s->current].init(&s->active_registers, (RETURN*) malloc(sizeof(RETURN)));

	return s;
}

////Duplicate a process (copied, NOT SHARED)
////The passed scheduler forks a process that is a duplicate
////of src_p.
int fork(SCHEDULER *s, PID src_p) {

}

////Overwrite the current process with the new information
////This exec is called on any PID that IS NOT 1!
////exec overwrites the given process with the new name, init, and step
int exec(SCHEDULER *s, PID pid, const char *new_name, PROCESS_CODE_PTR(init), PROCESS_CODE_PTR(step), int job_time) {

}



