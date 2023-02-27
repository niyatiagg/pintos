#ifndef USERPROG_PROCESS_H
#define USERPROG_PROCESS_H

#include "threads/thread.h"
typedef int pid_t;
typedef int tid_t;
#define PID_ERROR ((pid_t) -1)

struct p_c_b {
  pid_t pid;                          /* Process identifier. */
  struct list_elem child_elem;        /* List element for children list. */
  const char* file_name_copy;
  struct thread *parent;              /* Parent of the current thread */
  bool exited;                        /* Whether it has exited */
  bool waiting;                       /* Whether parent is waiting for its child. */
  bool orphaned;                      /* Whether parent alive or not */
  struct semaphore wait_sema;         /* Wait semaphore. */
  int exit_status;                    /* Status when it exits. */
};

tid_t process_execute (const char *file_name);
int process_wait (tid_t);
void process_exit (void);
void process_activate (void);

#endif /* userprog/process.h */
