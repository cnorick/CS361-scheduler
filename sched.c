#include "sched.h"
#include <stdlib.h>
#include <stdio.h>

PID add_process(SCHEDULER *s, PROCESS *p);
PROCESS *get_process(SCHEDULER *s, PID pid);
void saveActiveProcessState(SCHEDULER *s);
void goToNextProcess(SCHEDULER *s);

//Simulate a timer interrupt from hardware. This should initiate
////the context switch procedure
//// - Context switch must save active process' state into the PROCESS structure
//// - Context switch must load the next process' state into the scheduler
void timer_interrupt(SCHEDULER *s) {
	// call a function to save the active process's state 
	// into process structure
		// call process code ptr
			// how? What does this do?
		// update total_cpu_time to prev + clock cycle
		// update job_time
			// what is this?
		// save the current registers to saved_registers
		// set the process state as sleeping
	
	// function should also increment  current -> switched
		// is switching to a process and from an object both
		// things that increment current -> switched?
	
	// switch statement on s -> scheduler_algorithm
	// call the other functions that actually implement the algorithm
		// be sure to load the next 
}

void saveActiveProcessState(SCHEDULER *s){

}

void goToNextProcess(SCHEDULER *s){
	switch(s -> scheduler_algorithm){
		case SA_ROUND_ROBIN:
			break;
		case SA_FAIR:
			break;
		case SA_FCFS:
			break;
		case SA_SJF:
			break;
		default:
			break;
	}
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

    // Initialize process list with PS_NONE.
    PID i;
    for(i = 0; i < MAX_PROCESSES; i++)
        s->process_list[i].state = PS_NONE;

    // Load init into the scheculer and set it as running.
    s->current = add_process(s, init); // Should add it at 0, returning pid 1.
    s->active_registers = init->saved_registers;

    PROCESS *currentProcess = get_process(s, s->current);
    currentProcess->state = PS_RUNNING;
    currentProcess->init(&s->active_registers, (RETURN*) malloc(sizeof(RETURN)));

    return s;
}

////Duplicate a process (copied, NOT SHARED)
////The passed scheduler forks a process that is a duplicate
////of src_p.
int fork(SCHEDULER *s, PID src_p) {
    PROCESS *parent = get_process(s, src_p);

    PID newPid = add_process(s, parent);
    PROCESS *child = get_process(s, newPid);

    // Do we copy over time as well or make it 0 again?

    // Are we supposed to change this too?
    //child->state = PS_SLEEPING;

    return child->pid;
}

////Overwrite the current process with the new information
////This exec is called on any PID that IS NOT 1!
////exec overwrites the given process with the new name, init, and step
int exec(SCHEDULER *s, PID pid, const char *new_name, PROCESS_CODE_PTR(init), PROCESS_CODE_PTR(step), int job_time) {

}

// Adds a process to the scheduler in the first open process slot. Returns the pid of the new process (index + 1) or -1 if no slot was found.
PID add_process(SCHEDULER *s, PROCESS *p) {
    PID i;
    for(i = 0; i < MAX_PROCESSES; i++) {
        if(s->process_list[i].state == PS_NONE) {
            s->process_list[i] = *p;
            return i + 1;
        }
    }

    return -1;
}

// Returns the PROCESS with processId pid.
PROCESS *get_process(SCHEDULER *s, PID pid) {
    // process id is 1-based. array is 0-based.
    return &s->process_list[pid - 1]; 
}
