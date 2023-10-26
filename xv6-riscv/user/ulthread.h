#ifndef __UTHREAD_H__
#define __UTHREAD_H__

#include <stdbool.h>

#define MAXULTHREADS 100

enum ulthread_state {
  FREE,
  RUNNABLE,
  YIELD,
};

enum ulthread_scheduling_algorithm {
  ROUNDROBIN,
  PRIORITY,
  FCFS,         // first-come-first serve
};

struct cont {
  uint64 ra;
  uint64 sp;

  uint64 s0;
  uint64 s1;
  uint64 s2;
  uint64 s3;
  uint64 s4;
  uint64 s5;
  uint64 s6;
  uint64 s7;
  uint64 s8;
  uint64 s9;
  uint64 s10;
  uint64 s11;

  uint64 a0;
  uint64 a1;
  uint64 a2;
  uint64 a3;
  uint64 a4;
  uint64 a5;
  uint64 a6;
  uint64 a7;
};

struct thre {
  uint64 id;
  int priority;

  enum ulthread_state state;

  struct cont context;
};

struct round_robin_queue_element {
  struct thre* thread;
  struct round_robin_queue_element *next;
};

struct round_robin_queue {
  struct round_robin_queue_element *front;
  struct round_robin_queue_element *end;
};

struct ulthre {
  uint64 size;
  uint64 nexttid;

  enum ulthread_scheduling_algorithm sche_algo;

  struct thre* threads[MAXULTHREADS];
  struct round_robin_queue* rr_queue;
};

#endif