/* Here, we’re using clone() in two ways, once with the CLONE_VM flag and once without.
 * 
 * We start out by allocating a stack for the child process and then checking for the CLONE_VM
 * option. We then create a buffer of 100 bytes in the parent process and copy a string into it.
 *
 * Now if we execute the clone system call without CLONE_VM, the parent process' virtual memory
 * is cloned into the child pricess. The child process can access the message passed by the
 * parent process in buffer. But anything written into the buffer isn't accesible to the parent
 * process.
 * If we execute the clone system call with CLONE_VM flag, the child process shares the
 * parent's process memory. Now if we write into the buffer, it will be available to the parent
 * process
 */

//  We have to define the _GNU_SOURCE to get access to clone(2) and the CLONE_*
#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <unistd.h>

static int child_func(void* arg) {
  char* buffer = (char*)arg;
  printf("Child sees buffer = \"%s\"\n", buffer);
  strcpy(buffer, "hello from child");
  return 0;
}

int main(int argc, char** argv) {
  // Allocate stack for child task.
  const int STACK_SIZE = 65536;
  char* stack = malloc(STACK_SIZE);
  if (!stack) {
    perror("malloc");
    exit(1);
  }

  // When called with the command-line argument "vm", set the CLONE_VM flag on.
  unsigned long flags = 0;
  if (argc > 1 && !strcmp(argv[1], "vm")) {
    flags |= CLONE_VM;
  }

  int child_pid;
  char buffer[100];
  strcpy(buffer, "hello from parent");
  if ((child_pid = clone(child_func, stack + STACK_SIZE, flags | SIGCHLD, buffer)) == -1) {
    perror("clone");
    exit(1);
  }

  // Wait for the child process to finish
  printf("Hello from parent process! PID: %d, Child PID: %d\n", getpid(), child_pid);
  int status = waitpid(child_pid, NULL, 0);
  if (status == -1) {
    perror("wait");
    exit(1);
  }

  printf("Child exited with status %d. buffer = \"%s\"\n", status, buffer);
  return 0;
}
