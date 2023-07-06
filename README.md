
# OS Operating System Course Project


In the operating systems project I developed, the focus was on creating a comprehensive system that incorporated process management and memory management. The project aimed to simulate the behavior of an operating system by implementing various scheduling algorithms and memory management algorithms.

The project included different process scheduling algorithms such as Highest Priority First (HPF), Shortest Remaining Time (SRT), and Round Robin. These algorithms were implemented to manage the execution order and time allocation for processes. HPF prioritizes processes based on their importance, SRT selects the process with the shortest remaining time to execute, and Round Robin ensures fairness by allocating a fixed time slice to each process in a cyclic manner.

Furthermore, the project integrated different memory management algorithms like the Buddy Algorithm and First Fit Algorithm. The Buddy Algorithm divided the memory into blocks of varying sizes, allowing efficient allocation and deallocation of memory to processes. On the other hand, the First Fit Algorithm selected the first available memory block that could accommodate a process, thus optimizing memory utilization.

The project aimed to showcase the functionality and effectiveness of these scheduling and memory management algorithms in a simulated operating system environment. By simulating process execution and memory allocation, it provided insights into the performance and efficiency of different algorithms in handling varying workloads and memory requirements.

Overall, the project served as a valuable learning experience in understanding the intricacies of process management and memory management in operating systems. It provided a platform to explore the behavior and performance of different algorithms, enabling a deeper understanding of their strengths and weaknesses.

## System Structure:
# build

* [clk.out](.\build\clk.out)
* [ds.out](.\build\ds.out)
* [process.out](.\build\process.out)
* [processes.txt](.\build\processes.txt)
* [process_generator.out](.\build\process_generator.out)
* [scheduler.out](.\build\scheduler.out)
* [test_generator.out](.\build\test_generator.out)
code/
┣ .vscode/
┃ ┣ settings.json
┃ ┗ tasks.json
┣ data_structures/
┃ ┣ a.out
┃ ┣ bitmap.c
┃ ┣ main.c
┃ ┣ minHeap.c
┃ ┣ minHeap.h
┃ ┣ node.c
┃ ┣ node.h
┃ ┣ priorityQueue.c
┃ ┣ priorityQueue.h
┃ ┗ sequential_queue.h
┣ clk.c
┣ file
┣ headers.h
┣ Makefile
┣ pcb.h
┣ process.c
┣ process_generator.c
┣ ReadFile
┣ README
┣ README.md
┣ scheduler.c
┣ sfile
┣ shmfile
┣ test_generator
┣ test_generator.c
┗ WriteFile
Documents/
┗ Project_Summary.docx
io/
┣ memory.log
┣ processes.txt
┣ scheduler.log
┗ scheduler.perf
utils/
┣ build.bash
┣ p_gen.bash
┣ rmapp.bash
┗ run.bash

#### Input Files:
1. processes.txt:
   This includes the process id, arrival time, running time and priority.

#### Output Files:
1. perf.log:
   This includes utilization analysis of the scheduler, average waiting time and other calculations.
2. memory.log:
   This includes timing when the processes are allocated in memory and specifically where.
3. scheduler.log:
   This includes information of when processes are arrived, started and stopped with consideration of waiting time.
   

## Features

- Manage process using multiple scheduling algorithms
- Manage memory allocations of newly coming processes

## 🚀 About Us
We're a team in Cairo University, Faculty of Engineering, Computer Engineering Department. This Project is for Operating Systems Course.

## 🔗 Links
#### Contributers Social Links
1. Abdallah Abouelabbas
2. Omar Mahmoud Ezzat
3. Fares Atef
4. Mohammed Adel

