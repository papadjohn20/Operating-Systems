======================================================================
Project 3: 13/4/2026
Team:
Ioannis Papadopoulos 03899 papadjohn@uth.gr
Marios Sioros 04069 msioros@uth.gr
Charalampos Zachariadis 03734 czachariadis@uth.gr

----------------------------------------------------------------------
BUILD INSTRUCTIONS
----------------------------------------------------------------------
To compile the Working Set Monitor program, run the following command:

$ gcc wset_monitor.c -o wset_monitor

This will build the 'wset_monitor' executable, which interfaces with 
the custom system call implemented in the kernel.

----------------------------------------------------------------------
EXECUTION & CONFIGURATION
----------------------------------------------------------------------
To run the monitor:
$ ./wset_monitor

Customizing the Workload:
You can modify the following #define directives at the top of 
'wset_monitor.c' to test different scenarios before recompiling:

- MEM_SIZE: Memory region size (default 100MB).
- Workload_Access_Percentage: Percentage of pages to access randomly.
- MAX_ACTIVE_PAGES_TO_PRINT: Number of detailed page mappings to show.

----------------------------------------------------------------------
ANALYSIS AND RESULTS
----------------------------------------------------------------------
Detailed analysis regarding:
- The correctness of Physical Address translations.
- The verification of hardware flags (Present, Dirty, Young).
- Observations on Working Set results for 10%, 20%, 50%, and 90% workloads.

Can be found in the following file:
- report.txt

----------------------------------------------------------------------
FILES
----------------------------------------------------------------------
- patch_3         : Patch file containing all kernel-side modifications.
- wset_monitor.c  : User-space monitor application.
- report.txt      : Detailed analysis and verification report.
- README.txt      : This file.
======================================================================
