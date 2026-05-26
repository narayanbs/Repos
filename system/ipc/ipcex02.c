/*
 * alternate way of blocking signals
 * apart from sa_mask in signal handler
 * Note:
 * sa_mask is the preferred way
 */
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t got_usr1;
int counter = 0;

/*
 * your process receives SIGUSR1 this handler runs
 * during the handler someone presses Ctrl+C (SIGINT)
 * SIGINT is blocked, so it becomes pending
 * after restoration , kernel immediately delivers it
 * which usually terminates the process.
 */
void sigusr1_handler(int sig) {
  // Manually block signal without using sa_mask
  // Note: sa_mask is the preferred way
  sigset_t set, old;
  sigemptyset(&set);
  sigaddset(&set, SIGINT);
  sigprocmask(SIG_BLOCK, &set, &old);

  got_usr1 = 1;

  // Restore old mask
  sigprocmask(SIG_SETMASK, &old, NULL);
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

  // Manually block signals instead of sa_mask
  // Note: sa_mask is the preferred way

  /* Block SIGUSR1 */
  sigset_t set, old;
  sigemptyset(&set);
  sigaddset(&set, SIGUSR1);
  // change signal mask of current thread to block SIGUSR1
  // store old mask in old for restoration later
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
