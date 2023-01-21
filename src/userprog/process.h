#ifndef USERPROG_PROCESS_H
#define USERPROG_PROCESS_H

#include "threads/thread.h"
typedef int pid_t;
#define PID_ERROR ((pid_t) -1)

//struct pcb {
//    tid_t tid;                          /* Thread identifier. */
//  struct list_elem child_elem;        /* List element for children list. */
//    bool exited;                        /* Whether it has exited */
//    bool waited;                        /* Whether it has waited for some child. */
//    struct semaphore sema;         /* Wait semaphore. */
//    int exit_status;                    /* Status when it exits. */
//};

tid_t process_execute (const char *file_name);
int process_wait (tid_t);
void process_exit (void);
void process_activate (void);

#endif /* userprog/process.h */
