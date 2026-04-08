<div align=center>

# Operating Systems

[![Language](https://img.shields.io/badge/C-Language-yellow?style=for-the-badge)](https://github.com)

</div>

This repository contains my implementations for the laboratory exercises for the **Operating Systems** course ECE318 at the University of Thessaly from my 3rd.

<div align=center>

## 📂 Repository Structure

</div>


The course consists of 4 laboratory exercises. Each folder contains the source code and a detailed `README.txt` with compilation instructions and experimental results.

<div align=center>

| Project | Description |
| :--- | :--- |
| **Lab 1** | Custom System Calls & Kernel Modules (sysfs integration) |
| **Lab 2** | CPU Scheduling: Implementing Shortest Job First (SJF) |
| **Lab 3** | Memory Management: System Calls for Working Set Estimation |
| **Lab 4** | *Not published yet* |

</div>

---

<div align=center>

## 📝 Projects Overview

</div>

### Lab 1: System Calls & Kernel Modules
* **System Call:** Developed a new system call (`find_roots`) which performs a Process Tree Traversal. It identifies and logs the Process ID (PID) and Name of the calling process, as well as all its ancestors up to the `init` process.
* **Kernel Modules:** Modified the existing `Kyber I/O Scheduler` module gaining hands-on experience with kernel module compilation, loading/unloading (insmod/rmmod), and the multi-queue token-based algorithm.
* **Sysfs Integration:** Developed a custom kernel module that exports process information to user-space via the sysfs virtual filesystem. Reading from `/sys/kernel/teamXX/find_roots` triggers a lineage trace and returns the reader's PID.

#### 📂 Lab 1 Directory Structure
The files are organized into subdirectories based on the three parts of the assignment:
* `/project1_module`: Implementation of the modified Kyber I/O Scheduler kernel module.
* `/sysfs_module`: Source code for the sysfs kernel module for lineage tracing.
* `roots.c`, `roots.h`, `find_roots_lib.c`: Source files for the Process Tree library and the test application.
* `patch_1`: The kernel patch file containing the `find_roots` system call implementation.
* `README.txt`: Comprehensive guide for compilation, kernel loading, and team report.
* `Project_01.pdf`: The original assignment's PDF.


### Lab 2: Process Scheduling
* **SJF Implementation:** Modified the Linux scheduler to implement a variation of the `Shortest Job First (SJF)` algorithm.
* **Goodness Mechanism:** Enhanced the scheduler with a Goodness Value metric to ensure fairness. This prevents starvation of longer tasks by balancing CPU burst times with process aging/priority factors.
* **Evaluation:** Conducted performance profiling using specific workloads to compare execution times and scheduling efficiency.

#### 📂 Lab 2 Directory Structure
To maintain a clean repository, the files are organized as follows:
* `/src`: Contains only the modified source files (`schedule.c`, `schedule.h`).
* `/given_src`: Contains the given source files (`cpu.c`, `cpu.h`).
* `/tests`: Contains the `.conf` workload profiles used for testing.
* `/results`: Contains the `.txt` output files with performance metrics.
* `README.txt`: Serves as a guide of how to run the files and as a report for the report.
* `Project_02.pdf`: The assignment's pdf
* ` Makefile`

> **Note:** For compilation and execution, all files from the above directories must be placed in the same root folder as the provided simulator source code.

### Lab 3: Memory Management & Working Set Estimation

* **Kernel-Level Page Tracking:** Implemented a new system call (`sys_get_page_info`) that accepts a virtual address and returns detailed information about its corresponding physical page. The implementation supports retrieving page-level metadata and provides the functionality to clear the Accessed Bit at the hardware level.
* **Working Set Monitoring:** Developed a user-space application (`wset_monitor`) that calculates the Working Set Size of a process. The tool simulates various memory workloads and utilizes the custom syscall to track active vs. idle pages.
* **Experimental Evaluation & Reporting:** Conducted performance tests using `wset_monitor` across various memory access patterns (f.e., 10%, 20%, 50%, 90%). Verified the accuracy of physical address mapping and page flags returned by the system call, while documenting the correlation between estimated Working Set and actual memory access in a detailed `report.txt`.

#### 📂 Lab 3 Directory Structure
* `page_info.c`: The code containing the syscall implementation and page table traversal logic.
* `wset_monitor.c`: The code for the user-space applicationt. 
* `report.txt`: Detailed experimental results, observations on the "is_young" flag accuracy, and final conclusions.
* `README.txt`: Serves as a guide of how to run the files. 
* `Project_03.pdf`: The assignment's pdf

### Lab 4: Not published yet

---
