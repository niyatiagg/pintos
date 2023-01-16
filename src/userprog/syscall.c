#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "devices/shutdown.h"

static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
   /* int number;

    switch(number) {
        case SYS_HALT:
            halt ();
            break;
        case SYS_EXIT: exit();
        case SYS_EXEC:
        case SYS_WAIT:
        case SYS_CREATE:
        case SYS_REMOVE:
        case SYS_OPEN:
        case SYS_FILESIZE:
        case SYS_READ:
        case SYS_WRITE:
        case SYS_SEEK:
        case SYS_TELL:
        case SYS_CLOSE:

    } */
  printf ("system call!\n");
  thread_exit ();
}

/* Terminates Pintos

void
halt (void)
{
    shutdown_power_off ();
}

 Terminates the current user program, returning status to the kernel.

void
exit (int status)
{

}

pid_t
exec (const char *cmd_line)

int
wait (pid_t pid) */
