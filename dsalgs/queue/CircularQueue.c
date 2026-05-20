#include <stdio.h>

#define SIZE 5

int queue[SIZE];
int front = -1;
int rear = -1;

// Check if queue is full
int isFull() {
  return ((rear + 1) % SIZE == front);
}

// Check if queue is empty
int isEmpty() {
  return (front == -1);
}

// Insert element
void enqueue(int value) {
  if (isFull()) {
    printf("Queue is Full\n");
    return;
  }

  // First element
  if (front == -1) {
    front = rear = 0;
  } else {
    rear = (rear + 1) % SIZE;
  }

  queue[rear] = value;
  printf("%d inserted\n", value);
}

// Remove element
void dequeue() {
  if (isEmpty()) {
    printf("Queue is Empty\n");
    return;
  }

  printf("%d removed\n", queue[front]);

  // Only one element left
  if (front == rear) {
    front = rear = -1;
  } else {
    front = (front + 1) % SIZE;
  }
}

// Display queue elements
void display() {
  if (isEmpty()) {
    printf("Queue is Empty\n");
    return;
  }

  printf("Queue elements: ");

  int i = front;

  while (1) {
    printf("%d ", queue[i]);

    if (i == rear) break;

    i = (i + 1) % SIZE;
  }

  printf("\n");
}

// Get front element
void peek() {
  if (isEmpty()) {
    printf("Queue is Empty\n");
    return;
  }

  printf("Front element: %d\n", queue[front]);
}

int main() {
  enqueue(10);
  enqueue(20);
  enqueue(30);
  enqueue(40);

  display();

  dequeue();
  dequeue();

  display();

  enqueue(50);
  enqueue(60);

  display();

  peek();

  return 0;
}
