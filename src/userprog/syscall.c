#include "userprog/syscall.h"
#include <stdio.h>
#include <string.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "devices/shutdown.h"

static void syscall_handler (struct intr_frame *);
int sys_write(int fd, void *buffer, unsigned size);
void halt (void);
void exit (int status);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f)
{
    int number = *(int *)f->esp;

    switch(number) {
        case SYS_HALT:
            halt ();
            break;
        case SYS_EXIT: exit(0); break;
        case SYS_EXEC:
        case SYS_WAIT:
        case SYS_CREATE:
        case SYS_REMOVE:
        case SYS_OPEN:
        case SYS_FILESIZE:
        case SYS_READ:
        case SYS_WRITE: {
            int fd, ret;
            const void *buffer;
            unsigned size;
            memcpy(&fd, f->esp + 4, sizeof(fd));
            memcpy(&buffer, f->esp + 8, sizeof(buffer));
            memcpy(&size, f->esp + 12, sizeof(size));
            ret = sys_write(fd, buffer, size);
            f->eax = (uint32_t) ret;
            break;
        }
        case SYS_SEEK:
        case SYS_TELL:
        case SYS_CLOSE:;

    }
  printf ("system call!\n");
  thread_exit ();
}

int
sys_write(int fd, void *buffer, unsigned size) {
    int ret;
    if(fd == 1) { // write to stdout
        putbuf(buffer, size);
        ret = size;
    } else {
        ret = -1;
    }
    return ret;
}

void
halt (void)
{
  shutdown_power_off ();
}

void
exit (int status)
{

}
