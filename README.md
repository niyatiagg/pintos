PINTOS : It is an educational OS for the x86 architecture. Its a simple yet realistic enough approach on OS to get a grasp of concepts like multithreading, virtual memory and system calls.

Important Terms : 

    a) States of a thread : In the lifecycle of a thread, there are various states it assumes, namely :  THREAD_READY - Not running but ready to run; THREAD_RUNNING - Currently running thread; THREAD_BLOCKED - Waiting for an event to trigger; THREAD_DYING - About to be destroyed.
    b) Ready list : A list that stores elements of a thread(in ready state) that are scheduled to run but aren't running at present.

1. Project 1 : Alarm System : 
    I implemented alarm system in thread scheduling to avoid busy waiting (which is particularly helpful in systems with one cpu)
    
     -> In a call to timer_sleep, a call to thread_sleep, with the no.of ‘ticks' it must sleep for, is made where the current thread’s sleep ticks are updated to ’ticks'. That thread is pushed to the back of the 'sleep' list and is blocked. Every time the timer interrupt handler runs, it updates the threads in the 'sleep' list and monitors if they have slept enough, and adds them to the ready list if they have.
     
     -> Interrupts were turned off before the thread was blocked, curbing any attempts by another thread to be put to sleep simultaneously. It also prevents a race condition from timer interrupt.
     
     -> Absolute time v/s relative time : I changed the above implementation by replacing 'ticks'(relative time) in thread_sleep with total time(current time + ticks) i.e the time when thread wakes up. Insertion in sleep list is ordered according to their waking times. Earlier, with every tick we modified the entire sleep list. But now (at max) we just operate on the list for elements that need to be kicked off the list, else we stop. In practice, writing (to the 'sleep' list) is seldom in comparison to reading(iterate over the entire 'sleep' list) that happens at every tick. 
     
2. Project 2 : Priority Scheduling and donation : 
    Definition :
    
        a) Priority : An integer value (range - PRI_MIN (0) to PRI_MAX (63)) assigned to each thread.
        b) Priority Scheduling : arranging the threads according to their assigned priority.
        c) Priority donation : if a higher priority thread needs a lock which has been acquired by a lower priority thread, the former 'donates' its priorty to the latter in order to free the lock and acquire it for itself. 

    -> I implemented priority scheduling by ordering the ready list using a comparator (according to the thread's priority). Also, the current thread yields to a higher priority thread. 
    
    -> For donation, a holder attribute to the lock and a donated_lock attribute to the thread was added which is the pointer to the thread that currently has the lock and vice-versa for the latter. The thread with higher priority can donate its priority to the 'holder' of the lock. Upon release of the lock, the priority is then reset to its old priority that is stored in the thread's old priority attribute.
    
    -> While implementing multiple donation and nested donation, donated_lock attribute was replaced by waiting_lock attribute(for which lock the thread is blocked).
    
    
