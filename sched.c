#include "sched.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void saveActiveProcessRegisters(SCHEDULER *s);
void updateCurrentProcessTotalCPUTime(SCHEDULER *s);
void updateAllProcessSleepTimeRemaining(SCHEDULER *s);
void setCurrentProcessToSleeping(SCHEDULER *s);
void setNewCurrentProcess(SCHEDULER *s);
void setNextProcessToCurrent(SCHEDULER *s, unsigned int nextProcess);
void setCurrentProcessToRunning(SCHEDULER *s);
void loadActiveProcessRegisters(SCHEDULER *s);
void updateSwitchedCPUTime(SCHEDULER *s);
void initializeProcessSwitchedCPUTime(SCHEDULER *s);
void incrementCurrentProcessSwitched(SCHEDULER *s);
void callProcessCodePtrStep(SCHEDULER *s);
PID add_process(SCHEDULER *s, PROCESS *p);
PROCESS *get_process(SCHEDULER *s, PID pid);


//Simulate a timer interrupt from hardware. This should initiate
////the context switch procedure
//// - Context switch must save active process' state into the PROCESS structure
//// - Context switch must load the next process' state into the scheduler
void timer_interrupt(SCHEDULER *s) {
	saveActiveProcessRegisters(s);
	updateCurrentProcessTotalCPUTime(s);
	updateAllProcessSleepTimeRemaining(s);
	// setCurrentProcessToSleeping(s); maybe not?

	unsigned int oldCurrentProcess = s -> current; 
	
	setNewCurrentProcess(s);

	unsigned int newCurrentProcess = s -> current;
	
	// setCurrentProcessToRunning(s); maybe not?
	loadActiveProcessRegisters(s);

	if(newCurrentProcess == oldCurrentProcess){
		updateSwitchedCPUTime(s);
	}
	else{
		initializeProcessSwitchedCPUTime(s);
		incrementCurrentProcessSwitched(s);
		callProcessCodePtrStep(s);
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
    RETURN *r;

    // Create process 1.
    PROCESS init;
    init.name = "init";
    init.pid = 1;
    init.switched = 0;
    init.total_cpu_time = 0;
    init.switched_cpu_time = 0;
    init.sleep_time_remaining = 0;
    init.job_time = -1;
    init.state = PS_NONE;
    init.init = initCode;

    // Initialize process list with PS_NONE.
    PID i;
    for(i = 0; i < MAX_PROCESSES; i++)
        s->process_list[i].state = PS_NONE;

    // Load init into the scheculer and set it as running.
    s->current = add_process(s, &init) - 1; // Retruns pid. current needs the index into the array.
    s->active_registers = init.saved_registers;

    PROCESS *currentProcess = get_process(s, s->current + 1);
    currentProcess->state = PS_RUNNING;
    currentProcess->init(&s->active_registers, r);

    return s;
}

////Duplicate a process (copied, NOT SHARED)
////The passed scheduler forks a process that is a duplicate
////of src_p.
int fork(SCHEDULER *s, PID src_p) {
    PROCESS *parent = get_process(s, src_p);

    PID newPid = add_process(s, parent);
    PROCESS *child = get_process(s, newPid);

    // Reset all of the timers/counters for this new process.
    child->switched = 0;
    child->total_cpu_time = 0;
    child->switched_cpu_time = 0;
    child->sleep_time_remaining = 0;

    return child->pid;
}

////Overwrite the current process with the new information
////This exec is called on any PID that IS NOT 1!
////exec overwrites the given process with the new name, init, and step
int exec(SCHEDULER *s, PID pid, const char *new_name, PROCESS_CODE_PTR(init), PROCESS_CODE_PTR(step), int job_time) {
    // Don't try to exec init.
    if(pid == 1)
        return -1;
    
    // Get the process you want to exec.
    PROCESS *p = get_process(s, pid);
    if(p == NULL)
        return -1;

    // Reset all of the timers/counters.
    p->switched = 0;
    p->total_cpu_time = 0;
    p->switched_cpu_time = 0;
    p->sleep_time_remaining = 0;

    // Replace old name and function pointers.
    p->name = strdup(new_name);
    p->init = init;
    p->step = step;
    p->job_time = job_time;
}






/********************
 * Helper Functions *
 *******************/

// Adds a process to the scheduler in the first open process slot. Returns the pid of the new process (index + 1) or -1 if no slot was found.
PID add_process(SCHEDULER *s, PROCESS *p) {
    int i;
    for(i = 0; i < MAX_PROCESSES; i++) {
        if(s->process_list[i].state == PS_NONE) {
            s->process_list[i] = *p;
            s->process_list[i].pid = i + 1;
            return i + 1;
        }
    }

    return -1;
}

// Returns the PROCESS with processId pid.
PROCESS *get_process(SCHEDULER *s, PID pid) {
    if(pid > MAX_PROCESSES)
        return NULL;

    // process id is 1-based. array is 0-based.
    return &s->process_list[pid - 1]; 
}

void saveActiveProcessRegisters(SCHEDULER *s){

}

void updateCurrentProcessTotalCPUTime(SCHEDULER *s){

}

void updateAllProcessSleepTimeRemaining(SCHEDULER *s){

}

void setCurrentProcessToSleeping(SCHEDULER *s){

}

// all this does is choose set s -> current
// to what the next process should be
void setNewCurrentProcess(SCHEDULER *s){
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
			// maybe output an error?
			break;
	}

	// setNextProcessToCurrent(s, nextProcess);
}


void setNextProcessToCurrent(SCHEDULER *s, unsigned int nextProcess){

}

void setCurrentProcessToRunning(SCHEDULER *s){

}

void loadActiveProcessRegisters(SCHEDULER *s){

}

void updateSwitchedCPUTime(SCHEDULER *s){

}

void initializeProcessSwitchedCPUTime(SCHEDULER *s){

}

void incrementCurrentProcessSwitched(SCHEDULER *s){

}

void callProcessCodePtrStep(SCHEDULER *s){

}
