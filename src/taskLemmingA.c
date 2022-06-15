#include "syscall.h"
#include "i386.h"

void task(void) {
	while (1) {
		syscall_move(RIGHT);
		syscall_move(RIGHT);
		syscall_move(UP);
		syscall_move(LEFT);

		//syscall_explode();
		__asm volatile("nop");
	}
}
