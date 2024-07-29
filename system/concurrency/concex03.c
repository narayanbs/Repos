/*
 * Producer Consumer using conditional variable
 * pthread_cond_t
 */
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

typedef struct {
  char packet[30];
  bool transfer;
} Data;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void sendMsg(char*, Data*);
char* recvMsg(Data*);

void* produce(void* p) {
  Data* d = (Data*)p;
  char* packets[] = {"First packet", "Second packet", "Third packet", "Fourth packet", "End"};
  for (int i = 0; i < 5; i++) {
    sendMsg(packets[i], d);
    sleep(1);
  }
  return NULL;
}

void sendMsg(char* packet, Data* d) {
  pthread_mutex_lock(&mutex);
  while (d->transfer) {
    pthread_cond_wait(&cond, &mutex);
  }
  d->transfer = true;
  strcpy(d->packet, packet);
  pthread_cond_signal(&cond);
  pthread_mutex_unlock(&mutex);
}

void* consume(void* p) {
  Data* d = (Data*)p;
  for (;;) {
    char* msg = recvMsg(d);
    if (!strcmp(msg, "End")) {
      break;
    }
    printf("%s\n", msg);
    sleep(1);
  }
  return NULL;
}

char* recvMsg(Data* d) {
  pthread_mutex_lock(&mutex);
  while (!d->transfer) {
    pthread_cond_wait(&cond, &mutex);
  }
  d->transfer = false;
  char* packet = d->packet;
  pthread_cond_signal(&cond);
  pthread_mutex_unlock(&mutex);
  return packet;
}

int main(void) {
  Data d;
  d.transfer = false;
  pthread_t producer;
  pthread_t consumer;
  pthread_create(&producer, NULL, produce, &d);
  pthread_create(&consumer, NULL, consume, &d);
  pthread_join(producer, NULL);
  pthread_join(consumer, NULL);
  return 0;
}
