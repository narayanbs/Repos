/* Here, we’re using clone() in two ways, once with the CLONE_VM flag and once without.
 We’re passing a buffer into the child process, and the child process writes a string to it.
 We then allocate a stack size for the child process and create a function that checks whether
 we’re executing the file using the CLONE_VM (vm) option. Furthermore, we’re creating a buffer
 of 100 bytes in the parent process and copying a string to it, then executing
 the clone() system call and checking for errors.
 When we execute it without the vm argument, the CLONE_VM flag isn’t active,
 and the parent process virtual memory is cloned into the child process.
 The child process can access the message passed by the parent process in buffer,
 but anything written into buffer by the child isn’t accessible to the parent process.
 But, when we pass in the vm argument, CLONE_VM is active and the child process shares
 the parent’s process memory. We can see it writing into buffer
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
