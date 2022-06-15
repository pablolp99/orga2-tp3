#include "syscall.h"
#include "i386.h"

void task(void) {
  syscall_move(DOWN);
  syscall_move(DOWN);
  syscall_move(DOWN);
  syscall_move(LEFT);
  syscall_move(LEFT);
  syscall_move(LEFT);
  syscall_move(LEFT);
  syscall_move(LEFT);
  syscall_move(LEFT);
  syscall_move(LEFT);
  syscall_move(LEFT);
  syscall_move(LEFT);
  syscall_move(LEFT);
  syscall_move(LEFT);
  syscall_move(LEFT);
  while (1) {
	syscall_explode();
    __asm volatile("nop");
  }
}
