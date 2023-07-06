#pragma once
// struct pcb
// {
//     int id;
//     int arrival;
//     int runtime;
//     int priority;
// };

typedef enum {
    READY = 0,
    BLOCKED = 1,
    RUNNING = 2,
    FINISHED = 3
} PROC_STATE;

typedef struct pcb
{
    int id;
    int arrival;
    int runtime;
    int priority;
    int memidx;
    int memsize;
    PROC_STATE state;
    int pid;
    int starttime;
    int remainingtime;
} pcb;