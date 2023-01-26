#include "userprog/syscall.h"
#include <stdio.h>
#include <string.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "devices/shutdown.h"
#include "userprog/pagedir.h"
#include "threads/vaddr.h"

static void syscall_handler (struct intr_frame *);
int sys_write(int fd, void *buffer, unsigned size);
bool sys_create (const char *file, unsigned initial_size);
bool sys_remove (const char *file_name);
void halt (void);
void exit (int status);
pid_t exec (const char *file);
void *check_user_args (const void *uaddr);
static struct lock filesys_lock;

void
syscall_init (void) 
{
  lock_init(&filesys_lock);
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f)
{
    int number = *(int *)f->esp;

    switch(number) {
        case SYS_HALT: {
          halt ();
          break;
        }
        case SYS_EXIT: exit(0); break;
        case SYS_EXEC: {
          if(check_user_args(f->esp + 4) == NULL)
            thread_exit();

          char *cname = (char *)(f->esp + 4);
          exec(cname);
        }
        case SYS_WAIT:
        case SYS_CREATE: {
          bool ret;
          if(check_user_args(f->esp + 4) == NULL ||
              check_user_args(f->esp + 8) == NULL)
            thread_exit();

          char *file_name = (char *)(f->esp + 4);
          unsigned initial_size = (unsigned *)(f->esp + 8);
          ret = sys_create(file_name, initial_size);
          f->eax = ret;
          break;
        }
        case SYS_REMOVE: {
          bool ret;
          if(check_user_args(f->esp + 4) == NULL)
            thread_exit();

          char *file_name = (char *)(f->esp + 4);
          ret = sys_remove(file_name);
          f->eax = ret;
          break;
        }
        case SYS_OPEN: {
          if(check_user_args(f->esp + 4) == NULL)
            thread_exit();

          char *file_name = (char *)(f->esp + 4);
          //sys_open(file_name);
          break;
        }
        case SYS_FILESIZE:
        case SYS_READ:
        case SYS_WRITE: {
            int fd, ret;
            const void *buffer;
            unsigned size;
            if(check_user_args(f->esp + 4) == NULL ||
                check_user_args(f->esp + 8) == NULL ||
                  check_user_args(f->esp + 12) == NULL)
            {
                thread_exit();
            }

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
}

int
sys_write(int fd, void *buffer, unsigned size) {
    int ret;
    if(check_user_args(buffer) == NULL || check_user_args(((int *) buffer) + size-1 ) == NULL) {
        thread_exit ();
    }
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
  printf("%s: exit(%d)\n", thread_current ()->name, status);
  thread_exit();
}

void *
check_user_args (const void *uaddr)
{
  uint32_t *pd = active_pd ();
  if(!is_user_vaddr (uaddr))
      return NULL;

  return pagedir_get_page(pd, uaddr);
}

pid_t
exec (const char *file)
{
  return process_execute(file);
}

bool
sys_create (const char *file, unsigned initial_size)
{
  bool ret;
  lock_acquire (&filesys_lock);
  ret = filesys_create(file, initial_size);
  lock_release (&filesys_lock);
  return ret;
}

bool
sys_remove (const char *file_name)
{
  bool ret;
  lock_acquire (&filesys_lock);
  ret = filesys_remove(file_name);
  lock_release (&filesys_lock);
  return ret;
}