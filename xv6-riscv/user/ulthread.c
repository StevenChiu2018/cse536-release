/* CSE 536: User-Level Threading Library */
#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"
#include "user/ulthread.h"

/* Standard definitions */
#include <stdbool.h>
#include <stddef.h>

#define MAXARG 8

struct ulthre ulthread;
struct thre *cur_thread;

bool do_add_thread(struct thre*);
void set_args_to(struct cont *context, uint64 args[]);
uint64 recover_yield_thread(void);

/* Founctions for struct round_robin_queue*/
void push_thread_to_rrq(struct thre*);
struct thre* pop_thread_from_rrq(void);

/* Implementation of scheduling algo*/
struct thre* fcfs(void);
struct thre* priority(void);
struct thre* round_robin(void);

/* Get thread ID*/
uint64 get_current_tid() {
    return cur_thread->id;
}

/* Thread initialization */
void ulthread_init(int schedalgo) {
    struct thre *new_thread = malloc(sizeof(struct thre));
    new_thread->id = 0;
    new_thread->priority = 0;
    memset(&new_thread->context, 0, sizeof(new_thread->context));

    struct round_robin_queue *rrqueue = NULL;

    if(schedalgo == ROUNDROBIN) {
        rrqueue = malloc(sizeof(struct round_robin_queue));
    }

    ulthread.threads[0] = new_thread;
    ulthread.size = 1;
    ulthread.nexttid = 1;
    ulthread.sche_algo = schedalgo;
    ulthread.rr_queue = rrqueue;
    cur_thread = new_thread;
}

/* Thread creation */
bool ulthread_create(uint64 start, uint64 stack, uint64 args[], int priority) {
    struct thre *new_thread = malloc(sizeof(struct thre));

    new_thread->id = ulthread.nexttid;
    new_thread->state = RUNNABLE;
    new_thread->priority = priority;
    memset(&new_thread->context, 0, sizeof(new_thread->context));
    new_thread->context.ra = start;
    new_thread->context.sp = stack;
    set_args_to(&new_thread->context, args);

    if(!do_add_thread(new_thread)) {
        return false;
    }

    if(ulthread.sche_algo == ROUNDROBIN) {
        push_thread_to_rrq(new_thread);
    }

    ulthread.size++;
    ulthread.nexttid++;

    printf("[*] ultcreate(tid: %d, ra: %p, sp: %p)\n", new_thread->id, start, stack);

    return true;
}

void set_args_to(struct cont *context, uint64 args[]) {
    uint64 args_length = strlen(args);

    if(args_length > MAXARG) {
        args_length = MAXARG;
    }

    memmove(&context->a0, args, args_length);
}

bool do_add_thread(struct thre* thread) {
    if(ulthread.size < MAXULTHREADS) {
        ulthread.threads[ulthread.size] = thread;

        return true;
    }

    for(int i=1;i<MAXULTHREADS;i++) {
        if(ulthread.threads[i]->state == FREE) {
            ulthread.threads[i] = thread;

            return true;
        }
    }

    return false;
}

void push_thread_to_rrq(struct thre* thread) {
    struct round_robin_queue_element *new_rrqe = malloc(sizeof(struct round_robin_queue_element));

    new_rrqe->thread = thread;
    new_rrqe->next = NULL;

    if(ulthread.rr_queue->front) {
        ulthread.rr_queue->end->next = new_rrqe;
        ulthread.rr_queue->end = new_rrqe;
    } else {
        ulthread.rr_queue->front = new_rrqe;
        ulthread.rr_queue->end = new_rrqe;
    }
}

/* Thread scheduler */
void ulthread_schedule(void) {
    struct thre* chose_thread;

    for(;;) {
        switch (ulthread.sche_algo) {
            case FCFS:
                chose_thread = fcfs();
                break;

            case PRIORITY:
                chose_thread = priority();
                break;

            case ROUNDROBIN:
                chose_thread = round_robin();
                break;

            default:
                chose_thread = NULL;
                break;
        }

        uint64 recovered_yield_threads = recover_yield_thread();

        if(!chose_thread) {
            if(recovered_yield_threads == 0)
                return;

            continue;
        }

        cur_thread = chose_thread;

        /* Add this statement to denote which thread-id is being scheduled next */
        printf("[*] ultschedule (next tid: %d)\n", chose_thread->id);

        // Switch betwee thread contexts
        ulthread_context_switch(&ulthread.threads[0]->context, &chose_thread->context);
    }
}

struct thre* fcfs(void) {
    struct thre* chose_thread;

    for(int i=1;i<ulthread.size;i++) {
        if(ulthread.threads[i]->state != RUNNABLE) {
            continue;
        }

        if(!chose_thread) {
            chose_thread = ulthread.threads[i];
        }

        if(chose_thread->id > ulthread.threads[i]->id) {
            chose_thread = ulthread.threads[i];
        }
    }

    return chose_thread;
}

struct thre* priority(void) {
    struct thre* chose_thread;

    for(int i=1;i<ulthread.size;i++) {
        if(ulthread.threads[i]->state != RUNNABLE) {
            continue;
        }

        if(!chose_thread) {
            chose_thread = ulthread.threads[i];
        }

        if(chose_thread->priority < ulthread.threads[i]->priority) {
            chose_thread = ulthread.threads[i];
        }
    }

    return chose_thread;
}

struct thre* round_robin(void) {
    struct thre *chose_thread;
    int count = 0;

    while((count++ < ulthread.size) && (chose_thread = pop_thread_from_rrq()) && chose_thread) {
        if(chose_thread->state == FREE) {
            continue;
        }

        push_thread_to_rrq(chose_thread);

        if(chose_thread->state == RUNNABLE) {
            return chose_thread;
        }
    }

    return NULL;
}

struct thre* pop_thread_from_rrq(void) {
    if(!ulthread.rr_queue->front) {
        return NULL;
    }

    struct thre* thread = ulthread.rr_queue->front->thread;

    ulthread.rr_queue->front = ulthread.rr_queue->front->next;

    return thread;
}

uint64 recover_yield_thread(void) {
    uint64 count = 0;

    for(int i=1;i<ulthread.size;i++) {
        if(ulthread.threads[i]->state == YIELD) {
            ulthread.threads[i]->state = RUNNABLE;
            count++;
        }
    }

    return count;
}

/* Yield CPU time to some other thread. */
void ulthread_yield(void) {
    struct thre *yielded_thread = cur_thread;
    yielded_thread->state = YIELD;
    cur_thread = ulthread.threads[0];

    printf("[*] ultyield(tid: %d)\n", yielded_thread->id);
    ulthread_context_switch(&yielded_thread->context, &cur_thread->context);
}

/* Destroy thread */
void ulthread_destroy(void) {
    struct thre *destroyed_thread = cur_thread;
    destroyed_thread->state = FREE;
    cur_thread = ulthread.threads[0];

    printf("[*] ultdestroy(tid: %d)\n", destroyed_thread->id);
    ulthread_context_switch(&destroyed_thread->context, &cur_thread->context);
}