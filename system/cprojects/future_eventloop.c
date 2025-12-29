#include <stdio.h>   // Provides printf
#include <stdlib.h>  // Provides malloc and free

/* =========================
   Future
   ========================= */

/*
 * A Future represents a value that will be
 * available later (after an async task finishes).
 */
typedef struct {
  int completed;  // Flag indicating whether the task is done (0 = no, 1 = yes)
  void *result;   // Pointer to the result produced by the task
} Future;

/* =========================
   Task (state machine)
   ========================= */

/*
 * A Task represents a unit of work that progresses
 * step-by-step using a state machine.
 */
typedef struct Task {
  int (*step)(struct Task *);  // Function pointer for advancing the task
  void *state;                 // Pointer to task-specific state
  Future *future;              // Future to complete when task finishes
  struct Task *next;           // Next task in the linked list
} Task;

/* =========================
   Event Loop
   ========================= */

/*
 * The event loop holds a list of active tasks
 * and repeatedly runs them until all are complete.
 */
typedef struct {
  Task *head;  // Head of the linked list of tasks
} EventLoop;

/* =========================
   Future API
   ========================= */

/*
 * Allocate and initialize a new Future.
 */
Future *future_create(void) {
  Future *f = malloc(sizeof(Future));  // Allocate memory for the Future
  f->completed = 0;                    // Mark as not completed
  f->result = NULL;                    // No result yet
  return f;                            // Return the Future
}

/*
 * Mark a Future as completed and store its result.
 */
void future_complete(Future *f, void *result) {
  f->completed = 1;    // Mark the Future as completed
  f->result = result;  // Store the result
}

/* =========================
   Event Loop API
   ========================= */

/*
 * Create and initialize a new event loop.
 */
EventLoop *eventloop_create(void) {
  EventLoop *loop = malloc(sizeof(EventLoop));  // Allocate event loop
  loop->head = NULL;                            // No tasks yet
  return loop;                                  // Return event loop
}

/*
 * Add a task to the event loop.
 * Tasks are inserted at the head of the list.
 */
void eventloop_add(EventLoop *loop, Task *task) {
  task->next = loop->head;  // Link new task to current head
  loop->head = task;        // Update head to new task
}

/*
 * Run the event loop until all tasks finish.
 */
void eventloop_run(EventLoop *loop) {
  // Continue looping while there are tasks remaining
  while (loop->head) {
    Task **curr = &loop->head;  // Pointer to pointer for safe removal

    // Iterate through the task list
    while (*curr) {
      Task *task = *curr;  // Current task

      // Call the task's step function
      if (task->step(task)) {
        /* Task finished */
        *curr = task->next;  // Remove task from list
        free(task);          // Free task memory
      } else {
        curr = &task->next;  // Move to next task
      }
    }
  }
}

/* =========================
   Example async task
   ========================= */

/*
 * State structure for a counter task.
 */
typedef struct {
  int counter;  // Current counter value
} CounterState;

/*
 * Step function for the counter task.
 * Called repeatedly by the event loop.
 */
int counter_step(Task *task) {
  CounterState *s = task->state;  // Access task state

  // If the counter is still below 5, continue running
  if (s->counter < 5) {
    printf("Counter step %d\n", s->counter);  // Print current step
    s->counter++;                             // Increment counter
    return 0;                                 // Task not finished
  }

  // Allocate memory for the result
  int *result = malloc(sizeof(int));
  *result = s->counter;  // Store final counter value

  // Complete the future with the result
  future_complete(task->future, result);

  free(s);   // Free task state
  return 1;  // Task finished
}

/* =========================
   Main
   ========================= */

int main(void) {
  // Create the event loop
  EventLoop *loop = eventloop_create();

  // Create a Future to receive the result
  Future *f = future_create();

  // Allocate and initialize task state
  CounterState *state = malloc(sizeof(CounterState));
  state->counter = 0;

  // Allocate and initialize the task
  Task *task = malloc(sizeof(Task));
  task->step = counter_step;  // Assign step function
  task->state = state;        // Assign state
  task->future = f;           // Assign future

  // Add task to the event loop
  eventloop_add(loop, task);

  // Run the event loop until all tasks finish
  eventloop_run(loop);

  // Check if the future has completed
  if (f->completed) {
    int *value = (int *)f->result;  // Retrieve result
    printf("Future result: %d\n", *value);
    free(value);  // Free result memory
  }

  // Clean up remaining resources
  free(f);
  free(loop);
  return 0;  // Exit program successfully
}
