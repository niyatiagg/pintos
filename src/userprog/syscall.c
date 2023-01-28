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
int sys_open (const char *file_name);
int sys_filesize (int fd);
void sys_seek (int fd, unsigned position);
unsigned sys_tell (int fd);
void sys_close (int fd);
void halt (void);
void exit (int status);
pid_t exec (const char *file);
void *check_user_args (const void *uaddr);
static struct lock filesys_lock;


void
syscall_init (void) 
{
  lock_init (&filesys_lock);
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f)
{
  if(check_user_args(f->esp) == NULL || check_user_args(f->esp+3) == NULL)
    exit (-1);
    int number = *(int *)f->esp;

    switch(number) {
        case SYS_HALT: {
          halt ();
          break;
        }
        case SYS_EXIT:
        {
          if(check_user_args(f->esp + 4) == NULL)
            exit (-1);

          int status = *(int *)(f->esp + 4);
          exit(status);
          break;
        }
        case SYS_EXEC: {
          if(check_user_args(f->esp + 4) == NULL)
            exit (-1);

          pid_t ret;
          char *file_name;
          memcpy(&file_name, f->esp + 4, sizeof(file_name));
          ret = exec(file_name);
          f->eax = ret;
          break;
        }
        case SYS_WAIT:
        case SYS_CREATE: {
          bool ret;
          if(check_user_args(f->esp + 4) == NULL ||
              check_user_args(f->esp + 8) == NULL)
            exit (-1);

          char *file_name = *(char **)(f->esp + 4);
          unsigned initial_size = *(unsigned *)(f->esp + 8);
          ret = sys_create(file_name, initial_size);
          f->eax = ret;
          break;
        }
        case SYS_REMOVE: {
          bool ret;
          if(check_user_args(f->esp + 4) == NULL)
            exit (-1);

          char *file_name;
          memcpy(&file_name, f->esp + 4, sizeof(file_name));
          ret = sys_remove(file_name);
          f->eax = ret;
          break;
        }
        case SYS_OPEN: {
          if(check_user_args(f->esp + 4) == NULL)
            exit (-1);

          int ret;
          char *file_name;
          memcpy(&file_name, f->esp + 4, sizeof(file_name));
          ret = sys_open(file_name);
          f->eax = ret;
          break;
        }
        case SYS_FILESIZE:
        {
          if(check_user_args(f->esp + 4) == NULL)
            exit (-1);

          int fd = *(int *)(f->esp + 4);
          f->eax = sys_filesize (fd);
          break;
        }
        case SYS_READ:
        {
          int fd, ret;
          const void *buffer;
          unsigned size;
          if(check_user_args(f->esp + 4) == NULL ||
             check_user_args(f->esp + 8) == NULL ||
             check_user_args(f->esp + 12) == NULL)
          {
            exit (-1);
          }

          memcpy(&fd, f->esp + 4, sizeof(fd));
          memcpy(&buffer, f->esp + 8, sizeof(buffer));
          memcpy(&size, f->esp + 12, sizeof(size));
          ret = sys_read(fd, buffer, size);
          f->eax = (uint32_t) ret;
          break;
        }
        case SYS_WRITE: {
            int fd, ret;
            const void *buffer;
            unsigned size;
            if(check_user_args(f->esp + 4) == NULL ||
                check_user_args(f->esp + 8) == NULL ||
                  check_user_args(f->esp + 12) == NULL)
            {
                exit (-1);
            }

            memcpy(&fd, f->esp + 4, sizeof(fd));
            memcpy(&buffer, f->esp + 8, sizeof(buffer));
            memcpy(&size, f->esp + 12, sizeof(size));
            ret = sys_write(fd, buffer, size);
            f->eax = (uint32_t) ret;
            break;
        }
        case SYS_SEEK:
        {
          if(check_user_args(f->esp + 4) == NULL ||
              check_user_args(f->esp + 8) == NULL)
            exit (-1);

          int fd = *(int *)(f->esp + 4);
          unsigned position = *(unsigned *)(f->esp + 8);
          sys_seek (fd, position);
          break;
        }
        case SYS_TELL:
        {
          if(check_user_args(f->esp + 4) == NULL)
            exit (-1);

          int fd = *(int *)(f->esp + 4);
          f->eax = sys_tell (fd);
          break;
        }
        case SYS_CLOSE:
        {
          if(check_user_args(f->esp + 4) == NULL)
            exit (-1);

          int fd = *(int *)(f->esp + 4);
          sys_close(fd);
          break;
        }

    }
}

int
sys_read (int fd, void *buffer, unsigned size) {
  int ret;
  if(check_user_args(buffer) == NULL ||
     check_user_args((buffer) + size-1 ) == NULL)
  {
    exit (-1);
  }
  if(fd == 0) { // read from keyboard
    input_getc ();
    ret = size;
  } else if (fd < 128 && fd > 2){
    struct thread *t = thread_current ();
    lock_acquire (&filesys_lock);
    ret = file_read (t->file_d[fd], buffer, size);
    lock_release (&filesys_lock);
  }
  else exit(-1);
  return ret;
}

int
sys_write(int fd, void *buffer, unsigned size) {
    int ret;
    if(check_user_args(buffer) == NULL ||
        check_user_args((buffer) + size-1 ) == NULL)
    {
        exit (-1);
    }
    if(fd == 1) { // write to stdout
        putbuf(buffer, size);
        ret = size;
    } else if (fd < 128 && fd > 2)
    {
        struct thread *t = thread_current ();
        lock_acquire (&filesys_lock);
        ret = file_write (t->file_d[fd], buffer, size);
        lock_release (&filesys_lock);
    }
    else exit (-1);
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

pid_t
exec (const char *file_name)
{
  if (check_user_args(file_name) == NULL)
    exit (-1);

  return process_execute(file_name);
}

bool
sys_create (const char *file, unsigned initial_size)
{
  if (check_user_args(file) == NULL)
    exit (-1);

  bool ret;
  lock_acquire (&filesys_lock);
  ret = filesys_create(file, initial_size);
  lock_release (&filesys_lock);

  return ret;
}

bool
sys_remove (const char *file_name)
{
  if (check_user_args(file_name) == NULL)
    exit (-1);

  bool ret;
  lock_acquire (&filesys_lock);
  ret = filesys_remove(file_name);
  lock_release (&filesys_lock);

  return ret;
}

int
sys_open (const char *file_name)
{
  if (check_user_args(file_name) == NULL)
    exit (-1);

  struct thread *t = thread_current ();
  lock_acquire (&filesys_lock);
  struct file *f = filesys_open (file_name);
  lock_release (&filesys_lock);
  if(f == NULL) {
    return -1;
  }
  else {
    t->file_d[t->fd_next++] = f;
    return t->fd_next - 1;
  }
}

void
sys_close (int fd)
{
  if (fd > 0 && fd < 128 && thread_current ()->file_d[fd] != NULL) {
    lock_acquire (&filesys_lock);
    file_close (thread_current ()->file_d[fd]);
    lock_release (&filesys_lock);
    thread_current ()->file_d[fd] = NULL;
  }
  else exit (-1);
}

int
sys_filesize (int fd)
{
  return file_length (thread_current ()->file_d[fd]);
}

void
sys_seek (int fd, unsigned position)
{
  lock_acquire (&filesys_lock);
  file_seek (thread_current ()->file_d[fd], position);
  lock_release (&filesys_lock);
}

unsigned
sys_tell (int fd)
{
  return file_tell (thread_current ()->file_d[fd]);
}

void *
check_user_args (const void *uaddr)
{
  uint32_t *pd = active_pd ();
  if(!is_user_vaddr (uaddr))
    return NULL;

  return pagedir_get_page(pd, uaddr);
}
