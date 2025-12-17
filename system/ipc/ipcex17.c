#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {
  pid_t pid = fork();
  if (pid == -1) {
    perror("Fork failed");
    return 1;
  } else if (pid == 0) {
    printf("Child process started, pid: %d\n", getpid());
    sleep(10);
    printf("Child process exiting\n");
    exit(0);
  } else {
    sleep(2);
    printf("Parent: Sending SIGSTOP to child (pid %d)\n", pid);
    kill(pid, SIGSTOP);

    int status;
    waitpid(pid, &status, WUNTRACED);
    if (WIFSTOPPED(status)) {
      printf("Parent: Child process stopped (pid %d)\n", pid);
    }
    sleep(2);

    printf("Parent: Sending SIGCONT to child (pid %d)\n", pid);
    kill(pid, SIGCONT);

    waitpid(pid, &status, 0);
    if (WIFEXITED(status)) {
      printf("Parent: Child exited with status %d\n", WEXITSTATUS(status));
    }
  }
  return 0;
}
