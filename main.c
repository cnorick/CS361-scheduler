#include "sched.h"

void init(REGISTER_FILE *machine_registers, RETURN *r) {
	printf("starting init\n");
}

void p1Init(REGISTER_FILE *machine_registers, RETURN *r) {
	printf("starting p1\n");
}

void p1Step(REGISTER_FILE *machine_registers, RETURN *r) {
	printf("stepping p1\n");
}

void p2Init(REGISTER_FILE *machine_registers, RETURN *r) {
	printf("starting p2\n");
}

void p2Step(REGISTER_FILE *machine_registers, RETURN *r) {
	printf("stepping p2\n");
}

void p3Init(REGISTER_FILE *machine_registers, RETURN *r) {
	printf("starting p3\n");
}

void p3Step(REGISTER_FILE *machine_registers, RETURN *r) {
	printf("stepping p3\n");
}

void getInit() {

}


int main() {
	SCHEDULER *s = new_scheduler(init);

	// fork init.
	PID pid = fork(s, 1);
    exec(s, pid, "p1", p1Init, p1Step, 666);

    // Now fork p1.
    pid = fork(s, pid);
    exec(s, pid, "p2", p2Init, p2Step, 667);

    // fork init again.
    pid = fork(s, 1);
    exec(s, pid, "p3", p3Init, p3Step, 668);



	printf("\n");
	list_processes(s);


	return 0;
}
