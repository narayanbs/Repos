#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t got_usr1;
int counter = 0;

void sigusr1_handler(int sig) {
  got_usr1 = 1;
}

int main(void) {
  struct sigaction sa = {
      .sa_handler = sigusr1_handler,
      .sa_flags = 0,  // or SA_RESTART
      .sa_mask = 0,
  };

  got_usr1 = 0;

  if (sigaction(SIGUSR1, &sa, NULL) == -1) {
    perror("sigaction");
    exit(1);
  }

  printf("PID %d: ...\n", getpid());
  sigset_t set, old;

  /* Block SIGUSR1 */
  sigemptyset(&set);
  sigaddset(&set, SIGUSR1);
  sigprocmask(SIG_BLOCK, &set, &old);

  //------------critical section ----
  counter++;
  printf("counter = %d\n", counter);
  // doing some heavy work while we receive the signal
  sleep(15);

  /* Restore previous mask */
  sigprocmask(SIG_SETMASK, &old, NULL);

  while (!got_usr1) {
    printf("PID %d: working hard...\n", getpid());
    sleep(1);
  }

  printf("Done in by SIGUSR1!\n");

  return 0;
}
