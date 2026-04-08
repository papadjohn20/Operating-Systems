/* schedule.c
 * This file contains the primary logic for the 
 * scheduler.
 */
#include "schedule.h"
#include "macros.h"
#include "privatestructs.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

#define NEWTASKSLICE (NS_TO_JIFFIES(100000000))

/* Local Globals
 * rq - This is a pointer to the runqueue that the scheduler uses.
 * current - A pointer to the current running task.
 */
struct runqueue *rq;
struct task_struct *current;

/* External Globals
 * jiffies - A discrete unit of time used for scheduling.
 *			 There are HZ jiffies in a second, (HZ is 
 *			 declared in macros.h), and is usually
 *			 1 or 10 milliseconds.
 */
extern long long jiffies;
extern struct task_struct *idle;

/*-----------------Initilization/Shutdown Code-------------------*/
/* This code is not used by the scheduler, but by the virtual machine
 * to setup and destroy the scheduler cleanly.
 */
 
 /* initscheduler
  * Sets up and allocates memory for the scheduler, as well
  * as sets initial values. This function should also
  * set the initial effective priority for the "seed" task 
  * and enqueu it in the scheduler.
  * INPUT:
  * newrq - A pointer to an allocated rq to assign to your
  *			local rq.
  * seedTask - A pointer to a task to seed the scheduler and start
  * the simulation.
  */
void initschedule(struct runqueue *newrq, struct task_struct *seedTask)
{
	rq = newrq;

	seedTask->next = seedTask->prev = seedTask;
	newrq->head = seedTask;
	newrq->nr_running++;	
	
	seedTask->burst_start_time = 0;
	seedTask->last_burst = 0; 
	seedTask->exp_burst = 0;
	
	/* Set the initial time it entered the runqueue */
	seedTask->last_active_time = sched_clock();
}

/* killschedule
 * This function should free any memory that 
 * was allocated when setting up the runqueu.
 * It SHOULD NOT free the runqueue itself.
 */
void killschedule()
{
	return;   // Nothing to free in our implementation, but you may need to change this if you allocate memory in initschedule
}

/* print_rq
 * A helper function to print the contents of the runqueue.
*/
void print_rq () {
	struct task_struct *curr;
	
	printf("Rq: \n");
	curr = rq->head;
	if (curr)
		printf("%p", curr);
	while(curr->next != rq->head) {
		curr = curr->next;
		printf(", %p", curr);
	};
	printf("\n");
}

/* print_exp_bursts
 * A helper function to print the expected bursts of all tasks in the runqueue.
 * This is useful for debugging and understanding the state of the scheduler at any point in time.
 * It also calculates and shows the "Exp for comparison" which is the value used to compare against other tasks when deciding which task to run next.
*/
void print_exp_bursts(unsigned long long current_time) {
    struct task_struct *curr;
    
    /* Convert current_time from nanoseconds to milliseconds */
    printf("--- Scheduler Snapshot at %llums ---\n", current_time / 1000000);
    
    if (rq == NULL || rq->head == NULL) {
        printf("   RQ is empty.\n");
        return;
    }
    
    curr = rq->head->next;
    do {
        unsigned long long term1 = 3 * curr->exp_burst;
        unsigned long long term2 = 2 * curr->last_burst;
        unsigned long long prev_exp;

        if (term1 >= term2) {
            prev_exp = term1 - term2;
        } else {
            /* If the math would go negative, it means this is likely 
               an initial state or the first burst calculation. */
            prev_exp = 0; 
        }

        printf("  Task %-15s | Exp for comparison: %10llu | Last Burst: %10llu | Prev Exp: %10llu | NEW Exp: %10llu %s\n", 
               curr->thread_info->processName, 
			   (curr == current) ? (2 * (current_time - current->burst_start_time) + current->exp_burst) / 3 : curr->exp_burst,
			   curr->last_burst,
               prev_exp,
               curr->exp_burst,
               (curr == current) ? "[RUNNING]" : "");
               
        curr = curr->next;
    } while(curr != rq->head);
    
    printf("--------------------------------------\n");
}


/* print_goodness
 * A helper function to print the goodness scores of all tasks in the runqueue.
 * This is useful for debugging and understanding the state of the scheduler at any point in time when using the Goodness-based scheduling algorithm.
 * It calculates the goodness score for each task based on the formula:
 * Goodness(T) = ((Exp_Burst(T) + offset) / (Min_Exp_Burst + offset)) * ((Max_WaitingInRQ + offset) / (WaitingInRQ(T) + offset))
 * where offset is a constant to prevent division by zero and to scale the values appropriately.
*/
void print_goodness(unsigned long long now) {
    struct task_struct *curr;
    unsigned long long min_exp;
    unsigned long long max_wait;
	unsigned long long temp_exp;
	unsigned long long wait;
	unsigned long long t_exp;
	unsigned long long t_wait;
    double goodness;
    unsigned long long offset = 1000000; /* Intead of 1, we put 1ms */

    if (rq == NULL || rq->head == NULL) {
        printf("   RQ is empty.\n");
        return;
    }
    
    /* Find min_exp and max_wait */
    curr = rq->head->next;

    min_exp = curr->exp_burst;
    if (curr == current) {
        min_exp = (2 * (now - curr->burst_start_time) + curr->exp_burst) / 3;
		max_wait = 0;
    }
	else {
		max_wait = now - curr->last_active_time;
	}
    
    /* Iterate through the rest to find global min/max */
    curr = curr->next;
    while (curr != rq->head) {
        temp_exp = curr->exp_burst;
       
		if (curr == current) {
            temp_exp = (2 * (now - curr->burst_start_time) + curr->exp_burst) / 3;
			wait = 0;
		}
		else {
			wait = now - curr->last_active_time;
		}

        if (temp_exp < min_exp) {
			min_exp = temp_exp;
		}
        
        if (wait > max_wait) { 
			max_wait = wait;
		}

        curr = curr->next;
    }

    printf("\n--- Scheduler with Goodness Snapshot at %llums (MinExp: %llu, MaxWait: %llu) ---\n", 
           now / 1000000, min_exp, max_wait);

    /* Print the details and Goodness score for each task */
    curr = rq->head->next;
    while (curr != rq->head) {
        t_exp = curr->exp_burst;
        if (curr == current) {
            t_exp = (2 * (now - curr->burst_start_time) + curr->exp_burst) / 3;
			t_wait = 0;
        }
        else {
			t_wait = now - curr->last_active_time;
		}
                
        /* Calculate Goodness */
        goodness = ((double)(t_exp + offset) / (min_exp + offset)) * ((double)(max_wait + offset) / (t_wait + offset));

        printf(" Task: %-15s | Exp: %10llu | Wait: %10llu | Goodness: %10.4f %s\n",
               curr->thread_info->processName,
               t_exp,
               t_wait,
               goodness,
               (curr == current) ? "[RUNNING]" : "");

        curr = curr->next;
    }
    printf("----------------------------------------------------------------------------------\n");
}

/* schedule
 * This is the main scheduling function. It should decide which task to run next and perform a context switch to that task.
 * The scheduling decision should be based on the expected burst times of the tasks in the runqueue, and if you are implementing the Goodness-based scheduler, also based on their waiting times.
 */
void schedule() {
	unsigned long long now = sched_clock();
	struct task_struct *curr, *best_task = NULL;
	current->need_reschedule = 0;

	// if the only proccess running is init... do nothing
	if (rq->nr_running == 1) {
        context_switch(rq->head);
        return;
    }


	#ifdef USE_GOODNESS
	/*=========================================================*
	 *														   *
	 *		THIS WILL RUN ONLY IF YOU TYPE make goodness	   *
	 *														   *
	 *=========================================================*/
	
	unsigned long long min_exp;				/* minimum Expected Burst among all tasks in runqueue */
    unsigned long long max_wait;			/* maximum Waiting Time in RQ among all tasks in runqueue */
    unsigned long long wait_time;			/* variable to store the waiting time of the task currently being examined */
    unsigned long long temp_exp;			/* variable to store the calculated/temporary Expected Burst of a task */
	unsigned long long first_exp;           /* Expected Burst of the first task in the RQ (used for reference) */
	unsigned long long first_wait;          /* Waiting time of the first task in the RQ (used for reference) */
	double min_goodness;					/* minimum Goodness value found in the current scheduling round */
    double current_goodness;				/* variable to store the Goodness value of the task being examined */
	unsigned long long offset = 1000000;	/* Intead of 1, we put 1ms */

	print_goodness(now);

    curr = rq->head->next;
    
	min_exp = curr->exp_burst;
    
	/*Calculate min_exp and max_wait*/
	if (curr == current) {
        min_exp = (2 * (now - curr->burst_start_time) + curr->exp_burst) / 3;
		max_wait = 0;
    } else {
		max_wait = now - curr->last_active_time;
	}

    curr = curr->next;
    while (curr != rq->head) {
        temp_exp = curr->exp_burst;
        
		if (curr == current) { 
            temp_exp = (2 * (now - curr->burst_start_time) + curr->exp_burst) / 3;
			wait_time = 0;
        } else {
			wait_time = now - curr->last_active_time; 
		}

        if (temp_exp < min_exp) { 
			min_exp = temp_exp;
		}
		
        if (wait_time > max_wait) {  
            max_wait = wait_time;
        }
        curr = curr->next;
    }

    curr = rq->head->next;
    best_task = curr; 
    
	/* Temporary variables for the Goodness calculation of the first task */
    first_exp = best_task->exp_burst;
    if (best_task == current) {
        first_exp = (2 * (now - best_task->burst_start_time) + best_task->exp_burst) / 3;
		first_wait = 0;
    }
	else { 
		first_wait = now - best_task->last_active_time;
	}

    /* Initial min_goodness reference */
    min_goodness = ((double)(first_exp + offset) / (min_exp + offset)) * ((double)(max_wait + offset) / (first_wait + offset));

	/* Iterate through the rest of the queue to find the Min_Goodness */
    curr = curr->next;
    while (curr != rq->head) {
        temp_exp = curr->exp_burst;
        
		if (curr == current) {
            temp_exp = (2 * (now - curr->burst_start_time) + curr->exp_burst) / 3;
			wait_time = 0;
		}
		else {
			wait_time = now - curr->last_active_time;
		}

        current_goodness = ((double)(temp_exp + offset) / (min_exp + offset)) * ((double)(max_wait + offset) / (wait_time + offset));

        if (current_goodness < min_goodness) {
            min_goodness = current_goodness;
            best_task = curr;
        }
        curr = curr->next;
    }

    /* Safety Check: If current was deactivated, we must switch now */
    if (current->next == NULL) {
        best_task->burst_start_time = now;
        context_switch(best_task);
        return;
    }

	/* Decision: If we are switching to a different task */
    if (best_task != current) {
		/* If we are currently on 'init', always switch to a user task */
		/* Otherwise, only switch if the new task is strictly shorter */
        if (current == rq->head || min_goodness < 
            (((double)((2 * (now - current->burst_start_time) + current->exp_burst) / 3) + offset) / (min_exp + offset)) * ((double)(max_wait + offset) / (0 + offset))) {
            /* Update the statistics of the task leaving the CPU */
            current->last_burst = now - current->burst_start_time;
            current->exp_burst = (2 * current->last_burst + current->exp_burst) / 3;
            current->last_active_time = now;
			/* Keep track of the start time of the task entering the CPU*/
            best_task->burst_start_time = now;
            context_switch(best_task);
        }
    }
	#else
	/*=========================================================*
	 *														   *
	 *		   THIS WILL RUN ONLY IF YOU TYPE make         	   *
	 *														   *
	 *=========================================================*/
	unsigned long long min_exp_burst = 0;
    unsigned long long current_temp_exp;
    unsigned long long actual_exp;

    print_exp_bursts(now);

    current->need_reschedule = 0;

    if (rq->nr_running == 1) {
        context_switch(rq->head);
        return;
    } 
	else {
        
        current_temp_exp = (2 * (now - current->burst_start_time) + current->exp_burst) / 3;
		best_task = rq->head->next;

        min_exp_burst = (best_task == current) ? current_temp_exp : best_task->exp_burst;
		curr = best_task->next;
		while (curr != rq->head) {
           
            actual_exp = (curr == current) ? current_temp_exp : curr->exp_burst;
			if (actual_exp < min_exp_burst) {
				min_exp_burst = actual_exp;
				best_task = curr;
			}
			curr = curr->next;
		}

		if (current->next == NULL) {
			now = sched_clock();
			best_task->burst_start_time = now;
			context_switch(best_task);
			return;
		}

		if (best_task != current) {
			/* If we are currently on 'init', always switch to a user task */
			/* Otherwise, only switch if the new task is strictly shorter */
			if (current == rq->head || min_exp_burst < current_temp_exp) {
				now = sched_clock();
				/* Update the statistics of the task leaving the CPU */
				current->last_burst = now - current->burst_start_time;
				current->exp_burst = (2 * current->last_burst + current->exp_burst) / 3;
				current->last_active_time = now;
				/* Keep track of the start time of the task entering the CPU*/
				best_task->burst_start_time = now;
				context_switch(best_task);
			}
		}
	}
	#endif
}

/* sched_fork
 * Sets up schedule info for a newly forked task
*/
void sched_fork(struct task_struct *p)
{
	p->time_slice = 100;

	p->burst_start_time = 0;
	p->last_burst = 0;
	p->exp_burst = 0;

	p->last_active_time = sched_clock();
}

/* scheduler_tick
 * Updates information and priority
 * for the task that is currently running.
*/
void scheduler_tick(struct task_struct *p)
{
	schedule();
}

/* wake_up_new_task
 * Prepares information for a task
 * that is waking up for the first time
 * (being created).
 */
void wake_up_new_task(struct task_struct *p)
{	
	p->last_active_time = sched_clock();

	p->next = rq->head->next;
	p->prev = rq->head;
	p->next->prev = p;
	p->prev->next = p;
	
	rq->nr_running++;
}

/* activate_task
 * Activates a task that is being woken-up
 * from sleeping.
 */
void activate_task(struct task_struct *p)
{
	p->last_active_time = sched_clock();
	
	p->next = rq->head->next;
	p->prev = rq->head;
	p->next->prev = p;
	p->prev->next = p;
	
	rq->nr_running++;
}

/* deactivate_task
 * Removes a running task from the scheduler to
 * put it to sleep.
 */
void deactivate_task(struct task_struct *p)
{
	p->last_burst = sched_clock() - p->burst_start_time;
	p->exp_burst = (2 * p->last_burst + p->exp_burst) / 3;
	printf(" Deactivation with Exp: %10llu\n", p->exp_burst);
	p->prev->next = p->next;
	p->next->prev = p->prev;
	p->next = p->prev = NULL; /* Make sure to set them to NULL *
							   * next is checked in cpu.c      */

	rq->nr_running--;
}