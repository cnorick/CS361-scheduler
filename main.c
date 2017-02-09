#include "sched.h"

void init(REGISTER_FILE *machine_registers, RETURN *r) {
	printf("inside init\n");
}

int main() {
	SCHEDULER *s = new_scheduler(init);

	// fork init.
	fork(s, 1);
	fork(s, 1);
	fork(s, 1);
	fork(s, 1);

	list_processes(s);


	return 0;
}
