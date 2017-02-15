#include "sched.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void saveActiveProcessRegisters(SCHEDULER *s);
void setNewCurrentProcess(SCHEDULER *s);
void loadActiveProcessRegisters(SCHEDULER *s);
PID addProcess(SCHEDULER *s, PROCESS *p);
PROCESS *getProcess(SCHEDULER *s, PID pid);
PROCESS *getCurrentProcess(SCHEDULER *s);
void updateAllProcesses(SCHEDULER *s, RETURN r);
RETURN executeCurrentProcess(SCHEDULER *);
void putProcessToSleep(SCHEDULER *s, PID pid, unsigned int sleep_time);
void descheduleProcess(SCHEDULER *s, PID pid);
void initializeProcess(SCHEDULER *s, PID pid);
int getNumValidProcesses(SCHEDULER *s);
PID roundRobin(SCHEDULER *s);
PID fair(SCHEDULER *s);
PID fcfs(SCHEDULER *s);
PID sjf(SCHEDULER *s);


//Simulate a timer interrupt from hardware. This should initiate
////the context switch procedure
//// - Context switch must save active process' state into the PROCESS structure
//// - Context switch must load the next process' state into the scheduler
void timer_interrupt(SCHEDULER *s) {
    RETURN r; 
    
	saveActiveProcessRegisters(s);
    
    // Sets current to MAX_PROCESSES if no process can be scehduled.
	setNewCurrentProcess(s);

    if(s->current == MAX_PROCESSES)
        r.cpu_time_taken = 1;
    else {
        loadActiveProcessRegisters(s);
        r = executeCurrentProcess(s);
    }

    updateAllProcesses(s, r);
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
    PROCESS init;
    init.name = "init";
    init.pid = 1;
    init.switched = 0;
    init.total_cpu_time = 0;
    init.switched_cpu_time = 0;
    init.sleep_time_remaining = 0;
    init.job_time = -1;
    init.state = PS_RUNNING;
    init.init = initCode;
    init.step = initCode;

    // Initialize process list with PS_NONE.
    PID i;
    for(i = 0; i < MAX_PROCESSES; i++)
        s->process_list[i].state = PS_NONE;

    // Load init into the scheculer and set it as running.
    addProcess(s, &init);

    s->current = MAX_PROCESSES;

    return s;
}

////Duplicate a process (copied, NOT SHARED)
////The passed scheduler forks a process that is a duplicate
////of src_p.
int fork(SCHEDULER *s, PID src_p) {
    PROCESS *parent = getProcess(s, src_p);

    PID newPid = addProcess(s, parent);
    PROCESS *child = getProcess(s, newPid);

    // Reset all of the timers/counters for this new process.
    initializeProcess(s, newPid);

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
    PROCESS *p = getProcess(s, pid);
    if(p == NULL)
        return -1;

    // Reset counters.
    initializeProcess(s, pid);

    // Replace old name and function pointers.
    p->name = strdup(new_name);
    p->init = init;
    p->step = step;
    p->job_time = job_time;
}


/**************
 * Algorithms *
 **************/

PID roundRobin(SCHEDULER *s) {   
    int i;
    PROCESS p;

    for(i = s->current + 1; i < MAX_PROCESSES; i++) {
        p = s->process_list[i];

        if(p.state == PS_RUNNING)
            return p.pid;
    }
    for(i = 0; i < s->current; i++) {
        p = s->process_list[i];

        if(p.state == PS_RUNNING)
            return p.pid;
    }

    // No valid processes. We catch this already at the top of timer_interrupt.
    return MAX_PROCESSES;
}

PID fair(SCHEDULER *s) {
    int i;
    PROCESS *min = NULL;

    for(i = 0; i < MAX_PROCESSES; i++) {
        PROCESS *p = &s->process_list[i];

        if(p->state != PS_RUNNING)
            continue;
        
        if(min == NULL)
            min = p;

        double pTime = 
            p->switched_cpu_time == 0 ?
            (double)p->total_cpu_time :
            (double)p->total_cpu_time / (double)p->switched_cpu_time;

        double minTime = 
            min->switched_cpu_time == 0 ?
            (double)min->total_cpu_time :
            (double)min->total_cpu_time / (double)min->switched_cpu_time;

        if(pTime < minTime)
            min = p;
    }
    
    return min == NULL ? MAX_PROCESSES : min->pid;
}

PID fcfs(SCHEDULER *s) {
    int i; 
    for(i = 1; i < MAX_PROCESSES; i++) {
        PROCESS* p = &s->process_list[i];

        // Return the first process that is running.
        // This keeps the current process if it still isn't done.
        if(p->state == PS_RUNNING)
            return p->pid;
    }
    
    // If none are running.
    return MAX_PROCESSES;
}

PID sjf(SCHEDULER *s) {   
    int i;
    PROCESS *shortest = NULL;

    for(i = 1; i < MAX_PROCESSES; i++) {
        PROCESS *p = &s->process_list[i];

        if(p->state != PS_RUNNING)
            continue;
        
        if(shortest == NULL)
            shortest = p;

        if(p->job_time < shortest->job_time)
            shortest = p;
    }
    
    return shortest == NULL ? MAX_PROCESSES : shortest->pid;
}


/********************
 * Helper Functions *
 *******************/

// Adds a process to the scheduler in the first open process slot. Returns the pid of the new process (index + 1) or -1 if no slot was found.
PID addProcess(SCHEDULER *s, PROCESS *p) {
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
PROCESS *getProcess(SCHEDULER *s, PID pid) {
    if(pid > MAX_PROCESSES || pid < 1)
        return NULL;

    // process id is 1-based. array is 0-based.
    return &s->process_list[pid - 1]; 
}

// all this does is choose set s -> current
// to what the next process should be.
// If nothing can be scheduled, sets current
// to MAX_PROCESSES. 
void setNewCurrentProcess(SCHEDULER *s){
    PID pid = 0;

	switch(s -> scheduler_algorithm){
		case SA_ROUND_ROBIN:
            pid = roundRobin(s);
			break;
		case SA_FAIR:
            pid = fair(s);
			break;
		case SA_FCFS:
            pid = fcfs(s);
			break;
		case SA_SJF:
            pid = sjf(s);
			break;
	}
    
    // The algorithms return -1 if nothing can be scheduled.
    if(pid == -1) {
        s->current = MAX_PROCESSES;
        return;
    }

	// Set Next Process To Current.
    s->current = pid - 1;
}


void saveActiveProcessRegisters(SCHEDULER *s){
    PROCESS *pCur = getCurrentProcess(s);
    if(pCur == NULL)
        return;
    
    pCur->saved_registers = s->active_registers;
}

void loadActiveProcessRegisters(SCHEDULER *s){
    PROCESS *pCur = getCurrentProcess(s);
    if(pCur == NULL)
        return;

    s->active_registers = pCur->saved_registers;
}


PROCESS *getCurrentProcess(SCHEDULER *s) {
    return getProcess(s, s->current + 1);
}

// Updates the switched_cpu_time of all processes but the current, the sleep_time_remaining
// of sleeping process, and the total time for the current process. These values are
// incremented by elapsedTime. It also updates all the processes to be in the appropriate
// state.
void updateAllProcesses(SCHEDULER *s, RETURN r) {
    int i;
    for(i = 0; i < MAX_PROCESSES; i++) {
        PROCESS *p = s->process_list + i;

        // These processes don't exist.
        if(p->state == PS_NONE)
            continue;

        // This is the currently scheduled process.
        // If nothing was scheduled (current is MAX_PROCESSES), we'll never
        // reach the current process.
        else if(p->pid == s->current + 1) {
            // Increment elapsed time.
            p->total_cpu_time += r.cpu_time_taken;

            // Reset switched time since this one just ran.
            p->switched_cpu_time = 0;

            p->switched++;

            if(r.state == PS_SLEEPING)
                putProcessToSleep(s, p->pid, r.sleep_time);

            else if(r.state == PS_EXITED || (p->job_time != -1 && p->total_cpu_time >= p->job_time)) {
                descheduleProcess(s, p->pid);
            }

            continue;
        }


        // All other processes
        else {
            // Switched time is how long a process has been waiting.
            p->switched_cpu_time += r.cpu_time_taken;

            // If a process is sleeping, decrement its remaining sleep time.
            if(p->state == PS_SLEEPING) {
                // Decrement sleep_time_rmaining by elapsedTime.
                p->sleep_time_remaining -= r.cpu_time_taken;

                // Update its state if its sleep_time has elapsed.
                if(p->sleep_time_remaining <= 0)
                    p->state = PS_RUNNING;
            }
        }
    }
}

// Executes either step or init depending on whether or not the process has been run before.
RETURN executeCurrentProcess(SCHEDULER *s) {
    PROCESS *p = getCurrentProcess(s);
    RETURN r;

    // If process 1, return cpu_time_taken as 1 and state as running.
    if(p->pid == 1) {
        r.state = PS_RUNNING;
        r.cpu_time_taken = 1;
    }
    else if(p->total_cpu_time == 0)
        p->init(&s->active_registers, &r);
    else
        p->step(&s->active_registers, &r);

    return r;
}

void descheduleProcess(SCHEDULER *s, PID pid) {
    PROCESS *p = getProcess(s, pid);
    if(pid == 1) {
        perror("Cannot deschedule init!");
        exit(1);
    }

    p->state = PS_NONE;
}

void putProcessToSleep(SCHEDULER *s, PID pid, unsigned int sleep_time) {
    PROCESS *p = getProcess(s, pid);

    p->state = PS_SLEEPING;
    p->sleep_time_remaining = sleep_time;
}

// Initialize all of the process timers to zero.
void initializeProcess(SCHEDULER *s, PID pid) {
    PROCESS *p = getProcess(s, pid);

    // Reset all of the timers/counters.
    p->switched = 0;
    p->total_cpu_time = 0;
    p->switched_cpu_time = 0;
    p->sleep_time_remaining = 0;
}

// Return the number of running processes not including init.
// i.e. the number of options the scheduler has to switch to.
int getNumValidProcesses(SCHEDULER *s) {
    int num = 0, i;
    for(i = 1; i < MAX_PROCESSES; i++)
        num += (s->process_list[i].state == PS_RUNNING ? 1 : 0);

    return num;
}
