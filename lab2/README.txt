======================================================================
Project 2: 16/3/2026
----------------------------------------------------------------------
BUILD INSTRUCTIONS
----------------------------------------------------------------------
In the project we used the Makefile to switch between the two 
scheduling algorithms.

1. Standard SJF:
   To compile the default SJF scheduler, run:
   $ make
   This builds the 'sjf_sched' executable using the standard expected 
   burst logic.

2. Goodness-based SJF:
   To compile the version that incorporates "Goodness", run:
   $ make goodness
   This command automatically cleans the environment and recompiles 
   the source code with the USE_GOODNESS preprocessor flag enabled.

3. Cleaning:
   To remove all object files, libraries, and executables, run:
   $ make clean

----------------------------------------------------------------------
EXECUTION
----------------------------------------------------------------------
To run the scheduler with a specific profile configuration:
$ ./sjf_sched <profile_name>.conf

Example:
$ ./sjf_sched complex.conf

----------------------------------------------------------------------
PRINTING 
----------------------------------------------------------------------
Print Example of the default SJF scheduler:
The "Exp for comparison" is the value used to decide which task runs next.
For the currently running task, this value is specifically recalculated using its current execution time.
Also the print function shows the Last Burst, previous Exp Burst and New Exp of each task

--- Scheduler Snapshot at 40ms ---
  Task (Long_Task:3)   | Exp for comparison:    6666666 | Last Burst:   10000000 | Prev Exp:          0 | NEW Exp:    6666666 
  Task (Medium_Task:2) | Exp for comparison:    6666666 | Last Burst:   10000000 | Prev Exp:          0 | NEW Exp:    6666666 
  Task (Short_Task:1)  | Exp for comparison:    6666666 | Last Burst:          0 | Prev Exp:          0 | NEW Exp:          0 [RUNNING]

Print Example of the Goodness-based SJF scheduler:
It shows the minimum Exp Burst and the maximum WaitingInRQ in the first line , also the Exp Burst, WaitingInRQ and Goodness value of each task

--- Scheduler with Goodness Snapshot at 40ms (MinExp: 6666666, MaxWait: 20000000) ---
 Task: (Long_Task:3)   | Exp:    6666666 | Wait:   20000000 | Goodness:     1.0000 
 Task: (Medium_Task:2) | Exp:    6666666 | Wait:   10000000 | Goodness:     1.9091 
 Task: (Short_Task:1)  | Exp:    6666666 | Wait:          0 | Goodness:    21.0000 [RUNNING]

Both prints show which task was running in a given snapshot with the "[RUNNING]" string.
----------------------------------------------------------------------
TESTBENCH FILES
----------------------------------------------------------------------

1) test_non_interactive.conf:
This testbench file includes only non-interactive tasks.

2) test_interactive.conf:
This testbench file includes only interactive tasks.

3) test_starvation
This testbench file includes a very long tasks with much short tasks.
This tests if starvation occurs for the long task.

4) complex.conf: 
This testbench file includes a mix of long, medium, and short interactive and non-interactive tasks.
It has varying burst times and arrival times to test the scheduler's ability to handle diverse workloads efficiently.

----------------------------------------------------------------------
TESTBENCH RESULTS
----------------------------------------------------------------------

1) test_non_interactive.conf:

- minexp_test_non_interactive.txt:
All values were verified and the scheduler correctly picks the task with the smallest burst.
- goodness_test_non_interactive.txt:
The scheduler is more balanced here. Tasks switch more frequently because their waiting time grows.

2) test_interactive.conf:

- minexp_test_interactive.txt:
The calculations continue to work correctly, the deactivation/activation due to sleep/waking was verified.
- goodness_test_interactive.txt:
The scheduler continues to be more balanced here.

3) test_starvation

- minexp_test_starvation.txt: 
The trace demonstrates pure SJF continuously prioritizing the short, interactive "Asterix" tasks over the long "Obelix" process.
Because the Asterix tasks frequently sleep and wake with low expected burst times, Obelix is constantly preempted and left lingering in the run queue.

- goodness_test_starvation.txt:
As "Obelix" sits in the queue, its Goodness score decreases, which periodically forces the scheduler to preempt the shorter "Asterix" tasks and grant the heavy process necessary CPU time.
This completely eliminates the starvation seen in standard SJF successfully balancing long, CPU-bound jobs with frequent, interactive ones.

4) complex.conf: 

- minexp_complex.txt:
Verified the parent and child behavior, also the scheduler continues to work correctly even with a longer and more complex profile.
- goodness_complex.txt:
Once again similar behavior but more balanced.

----------------------------------------------------------------------
FILES
----------------------------------------------------------------------
- schedule.c , schedule.h
- Makefile 
- complex.conf , test_interactive.conf , test_non_interactive.conf , test_starvation.conf
- minexp_complex.txt , minexp_test_interactive.txt , minexp_test_non_interactive.txt , minexp_test_starvation.txt
- goodness_complex.txt , goodness_test_interactive.txt , goodness_test_non_interactive.txt , goodness_test_starvation.txt
- README.txt
======================================================================
