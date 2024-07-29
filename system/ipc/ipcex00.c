#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {
  pid_t pid;
  int rv;

  switch (pid = fork()) {
  case -1:
    perror("fork");
    exit(1);
  case 0: // child
    printf(" CHILD: This is the child process!\n");
    printf(" CHILD: My PID is %d\n", getpid());
    printf(" CHILD: My parent's PID is %d\n", getppid());
    printf(" CHILD: Enter my exit status (make it small): ");
    scanf(" %d", &rv);
    printf(" CHILD: I'm outta here!\n");
    exit(rv);
  default: // parent
    printf("PARENT: This is the parent process!\n");
    printf("PARENT: My PID is %d\n", getpid());
    printf("PARENT: My child's PID is %d\n", pid);
    printf("PARENT: I'm now waiting for my child to exit()...\n");
    wait(&rv);
    printf("PARENT: My child's exit status is: %d\n", WEXITSTATUS(rv));
    printf("PARENT: I'm outta here!\n");
  }
  /* Normally this is how a fork call would be used
    if (!fork()) {
      printf("I'm the child!\n");
      exit(0);
    } else {
      printf("I'm the parent!\n");
      wait(NULL);
    }
  */
  return 0;
}
