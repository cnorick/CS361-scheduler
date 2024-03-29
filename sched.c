#include "sched.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define INVALID_PID MAX_PROCESSES + 1

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

    // Initialize all the mutexes as not existing.
    for(i = 0; i < MAX_MUTEX; i++)
        s->mutex_list[i] = -1;

    // Initialize all the semaphores as not existing.
    for(i = 0; i < MAX_SEM; i++)
        s->sem_list[i] = -1;

    // Load init into the scheduler and set it as running.
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
    if(newPid == INVALID_PID)
        return -1;

    PROCESS *child = getProcess(s, newPid);

    // Reset all of the timers/counters for this new process.
    initializeProcess(s, newPid);

    return child->pid;
}

////Overwrite the current process with the new information
////This exec is called on any PID that IS NOT 1!
////exec overwrites the given process with the new name, init, and step
////Returns 0 on failure. Non-zero on success.
int exec(SCHEDULER *s, PID pid, const char *new_name, PROCESS_CODE_PTR(init), PROCESS_CODE_PTR(step), int job_time) {
    // Don't try to exec init.
    if(pid == 1)
        return 0;
    
    // Get the process you want to exec.
    PROCESS *p = getProcess(s, pid);
    if(p == NULL)
        return 0;
    if(p->state == PS_NONE)
        return 0;

    // Reset counters.
    initializeProcess(s, pid);

    // Replace old name and function pointers.
    p->name = strdup(new_name);
    p->init = init;
    p->step = step;
    p->job_time = job_time;

    return 1;
}

// Mutex definitions:
// -1: There is no mutex at that index.
// 0: Mutex is created, but not locked.
// 1: Mutex is locked.

// Returns mutex id or -1 if one can't be allocated.
MUTEX mutex_create(SCHEDULER *s) {
    int i;
    for(i = 0; i < MAX_MUTEX; i++) {
        if(s->mutex_list[i] == -1) {
            s->mutex_list[i] = 0;
            return i;
        }
    }
    // None can be created.
    return -1;
}

// If m is in bounds, set the mutex at that index to -1 (non-existent).
void mutex_destroy(SCHEDULER *s, MUTEX m) {
    if(m < 0 || m >= MAX_MUTEX)
        return;

    s->mutex_list[m] = -1;
}

//Lock or unlock a mutex given by the mutex index m
//If a lock cannot be granted, return 0, otherwise return 1
int mutex_lock(SCHEDULER *s, MUTEX m) {
    if(m < 0 || m >= MAX_MUTEX)
        return 0;

    // If it's unlocked...
    else if(s->mutex_list[m] == 0) {
        s->mutex_list[m] = 1;
        return 1;
    }

    // If it's locked already, or it doesn't exist at all...
    else return 0;
}

void mutex_unlock(SCHEDULER *s, MUTEX m) {
    if(m < 0 || m >= MAX_MUTEX)
        return;

    // If it's locked...
    else if(s->mutex_list[m] == 1) {
        s->mutex_list[m] = 0;
    }

    // If it's already unlocked or doesn't exist, then do nothing.
}


// Semaphore definitions:
// -1: There is no semaphore at that index.
// >= 0: The semaphore exists and has a number of what the value is at the index.

// Returns semaphore id or -1 if one can't be allocated.
SEMAPHORE sem_create(SCHEDULER *s) {
    int i;
    for(i = 0; i < MAX_SEM; i++) {
        if(s->sem_list[i] == -1) {
            s->sem_list[i] = 0;
            return i;
        }
    }
    // None can be created.
    return -1;
}

// if m is in bounds, set the semaphore at m to -1.
void sem_destroy(SCHEDULER *s, SEMAPHORE m) {
    if(m < 0 || m >= MAX_SEM)
        return;

    s->sem_list[m] = -1;
}

//Raises or lowers the semaphore number of semaphore m
void sem_up(SCHEDULER *s, SEMAPHORE m) {
    // Check bounds.
    if(m < 0 || m >= MAX_SEM)
        return;
    
    // Check that semaphore exists.
    if(s->sem_list[m] == -1)
        return;

    s->sem_list[m]++;
}

// Returns the number of resources remaining BEFORE the count is decremented.
int sem_down(SCHEDULER *s, SEMAPHORE m) {
    // Check bounds.
    if(m < 0 || m >= MAX_SEM)
        return 0;
    
    else if(s->sem_list[m] > 0) {
        // Post-decrement because we want to return > 0 if there is a resource left.
        return s->sem_list[m]--;
    }
    // For those non-existent and those that == 0.
    else return 0;
}


/**************
 * Algorithms *
 **************/

PID roundRobin(SCHEDULER *s) {   
    int i;
    PROCESS p;

    // Check all processes except current first.
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

    // If none of the other process are available to run, schedule current.
    p = *getCurrentProcess(s);
    if(p.state == PS_RUNNING)
        return p.pid;

    // No valid processes. We catch this already at the top of timer_interrupt.
    return MAX_PROCESSES + 1;
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
    
    return min == NULL ? MAX_PROCESSES + 1 : min->pid;
}

PID fcfs(SCHEDULER *s) {
    int i; 
    for(i = 1; i < MAX_PROCESSES; i++) {
        PROCESS* p = &s->process_list[i];

        // Return the first process that is running.
        // This keeps the current process if it still isn't done.
        // If it's sleeping, we can't schedule it, but we also can't go to another process until the current one exits.
        if(p->state == PS_RUNNING)
            return p->pid;
        else if(p->state == PS_SLEEPING)
            return MAX_PROCESSES + 1;
    }
    
    // If none are running.
    return MAX_PROCESSES + 1;
}

PID sjf(SCHEDULER *s) {   
    int i;
    PROCESS *shortest = NULL;

    for(i = 1; i < MAX_PROCESSES; i++) {
        PROCESS *p = &s->process_list[i];

        if(!(p->state == PS_RUNNING || p->state == PS_SLEEPING))
            continue;
        
        if(shortest == NULL)
            shortest = p;

        if(p->job_time < shortest->job_time)
            shortest = p;
    }
    
    if(shortest == NULL || shortest->state == PS_SLEEPING)
        return MAX_PROCESSES + 1;
    else
        return shortest->pid;
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

    return INVALID_PID;
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
                
                // Update its state if its sleep_time has elapsed.
                if((int)p->sleep_time_remaining - (int)r.cpu_time_taken <= 0) {
                    p->state = PS_RUNNING;
                    p->sleep_time_remaining = 0;
                    continue;
                }

                // Decrement sleep_time_rmaining by elapsedTime.
                p->sleep_time_remaining -= r.cpu_time_taken;
            }
        }
    }
}

// Executes either step or init depending on whether or not the process has been run before.
RETURN executeCurrentProcess(SCHEDULER *s) {
    PROCESS *p = getCurrentProcess(s);
    RETURN r;

    if(p->total_cpu_time == 0)
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
