#include "headers.h"
#include "data_structures/sequential_queue.h"
#include "data_structures/priorityQueue.h"
#include "data_structures/bitmap.c"
#include "math.h"
// add comments to this file

/**
 * Scheduler Type:
 *  > Initialized after receiving from the message queue the scheduler type from process generator module
 */

float WTA_Sum = 0;
int Wait_Sum = 0;
int usedtime = 0;
int lastFinishedTime = 0;
int FinishedProcesses = 0;

SCH_TYPE schedulerType;
Queue *RRQ, *FQ,*waitingList;
priorityQueue *p_q;
pcb *prev_process;
// int *process_state_addr;
int Quantum = 0;
int startRunTime = 0;
// int shmScId;
int shmid;

int MPrevClk = -1;

Bitmap* mem;
pcb *current_process;
FILE *f, *fperf;
int semR, semW;
void *schshmaddr;

int memtype;

int pclk = -1;
int flag = 0;

bool no_more_processes = 0;

int temp_start = 0, temp_remaining;
// This function is a signal handler for process termination
void processTerminationHandler(int signum)
{
    // Check if there is a current process running
    if (current_process)
    {
        // Print the process details, including its ID, arrival time, runtime, and finished time
        printf("Scheduler:\t\tProcess <%d> has ended at time(%d), arr(%d), rt(%d)\n", current_process->id, getClk(), current_process->arrival, current_process->runtime);

        // Freeing Allocated Memory
        if(memtype == 1)
            clearBlock(mem,current_process->memidx,current_process->memsize);      
        else
            clearBuddyBlock(mem,current_process->memidx,current_process->memsize);      

        // Calculate the process's weighted turnaround time (WTA)
        float WTA = ((float)getClk() - current_process->arrival) / current_process->runtime;

        // Calculate the process's waiting time
        int wait = getClk() - current_process->runtime - current_process->arrival;

        // To Be Released Upon Queue Destruction
        float *WTA_TMP = malloc(sizeof(float *));
        *WTA_TMP = WTA;
        queue_push(FQ, WTA_TMP);

        // Update the WTA and waiting time sums, as well as the used CPU time
        WTA_Sum += WTA;
        Wait_Sum += wait;
        usedtime += current_process->runtime;
        lastFinishedTime = getClk();

        // Increment the number of finished processes
        FinishedProcesses++;

        // Calculate the standard deviation of the WTAs
        float sumOfSquares = 0;
        for (int i = 0; i < FinishedProcesses; i++)
        {
            // Get the WTA from the feedback queue and print it
            float *_WTA = (float *)queue_top(FQ);

            // Calculate the difference between the WTA and the average WTA
            float diff = (*_WTA - ((float)WTA_Sum / FinishedProcesses));

            // Add the squared difference to the sum of squares
            sumOfSquares += diff * diff;

            // Move the WTA to the back of the feedback queue (Cycle)
            queue_pop(FQ);
            queue_push(FQ, _WTA);
        }

        // Calculate the standard deviation and print the results
        float std = sqrt((float)sumOfSquares / FinishedProcesses);

        // Write the performance metrics to a file
        fperf = fopen("../io/scheduler.perf", "w");
        fprintf(fperf, "CPU Utilization = %.2f%% \nAvg WTA = %.2f \nAvg Waiting = %.2f \nStd WTA = %.2f \n",
                ((float)usedtime / lastFinishedTime) * 100, (float)WTA_Sum / FinishedProcesses, (float)Wait_Sum / FinishedProcesses, std);
        fclose(fperf);
        current_process->remainingtime = temp_remaining - getClk() + temp_start;
        setSpecificKeyValue(p_q, current_process->id, current_process->remainingtime);
        // Write the process details to a log file
        f = fopen("../io/scheduler.log", "a+");
        fprintf(f, "At time %d process %d %s arr %d total %d remain %d wait %d TA %d WTA %.2f\n",
                getClk(), current_process->id,
                "finished", current_process->arrival, current_process->runtime, current_process->remainingtime,
                wait, getClk() - current_process->arrival, WTA);
        fclose(f);

        // Write the process details to a log file
        f = fopen("../io/memory.log", "a+");
        fprintf(f, "At time %d %s %d from process %d from %d to %d\n",
                getClk(), 
                "freed", current_process->memsize,current_process->id, current_process->memidx,
                ((memtype == 1) ? current_process->memidx+ current_process->memsize:current_process->memidx+roundToPowerOf2(current_process->memsize)) - 1);
        fclose(f);

        // Update the current process's state to finished
        current_process->state = FINISHED;
        // if(schedulerType != RR) free(current_process);
    }

    // If the scheduler type is round-robin, remove the current process from the round-robin queue
    if (schedulerType == RR)
    {
        current_process = NULL;
        prev_process = NULL;
        queue_pop(RRQ);
    }
    // If the scheduler type is priority-based, remove the current process from the priority queue
    else
    {
        pcb *deleted;
        deleted = dequeue(p_q);
    }
}

// This function releases shared memory and semaphores resources and exits the program
void releaseResources()
{   
    // Bit Map
    //freeBitmap(mem);
    // Used Queues
    destroyPriorityQueue(p_q);
    Queue_Finalize(RRQ);
    Queue_Finalize(FQ);
    // Semaphores
    semctl(semR, 0, IPC_RMID, 0);
    semctl(semW, 0, IPC_RMID, 0);
    // Shared Memory
    shmctl(shmid, IPC_RMID, 0);
    // Clock
    destroyClk(true);
    // Exit Code
    exit(0);
}

void readProcesses(int signum)
{
    // clock instance to manage memory allocation
    int clock = getClk();
    // Verify that readProcesses is called once
    if (clock <= MPrevClk)  return;
    // Check if there's process/es to read
    if(!semctl(semR, 0, GETVAL)) return;

    // Update the previous clock
    MPrevClk = clock;
        
    // Start Reading Process/es
    down(semR);

    // Get the number of processes in shared memory
    int count = *((int *)schshmaddr + 1);
    
    // Allow writing to shared memory
    up(semW);

    if (count == -1)
    {
        no_more_processes = 1;
        return;
    }

    // Loop through all the processes in shared memory
    for (int i = 0; i < count; i++)
    {
        // Wait for permission to read from shared memory
        down(semR);

        // Read the process from shared memory and make a copy of it
        pcb CopyProcess = *((pcb *)((int *)schshmaddr + 2));
        pcb *process = malloc(sizeof(pcb));
        process->arrival = CopyProcess.arrival;
        process->id = CopyProcess.id;
        process->priority = CopyProcess.priority;
        process->runtime = CopyProcess.runtime;
        process->memsize = CopyProcess.memsize;
        process->state = CopyProcess.state;
        process->remainingtime = CopyProcess.remainingtime;

        // Initialize the process state to READY
        process->state = READY;
        queue_push(waitingList,process);

        up(semW);

    }

}

int main(int argc, char *argv[])
{
    // Signal Handlers Assignment
    signal(SIGINT, releaseResources);
    signal(SIGUSR1, readProcesses);
    signal(SIGUSR2, processTerminationHandler);


    // Initialization Queues
    p_q = createPriorityQueue(1000);
    RRQ = Queue_Init();
    waitingList = Queue_Init();
    FQ = Queue_Init();

    // Memory Allocation and initialization
    mem = malloc(sizeof(Bitmap));
    initBitmap(mem,1024);

    key_t shm_key = ftok("../code/shmfile", 65);
    // Process Memory Logic
    //      4 bytes     --> Type of message (Set Scheduler Type, Send Process)
    //      4 bytes     --> OPT: Process ID
    //      16 bytes    --> Data (SCH_TYPE type|| pcb* process ptr)
    shmid = shmget(shm_key, sizeof(int) * 2 + sizeof(pcb), IPC_CREAT | 0666);
    schshmaddr = shmat(shmid, (void *)0, 0);

    // Get Semaphore ids
    key_t semReadKey = ftok("../code/ReadFile", 65);
    key_t semWriteKey = ftok("../code/WriteFile", 65);
    semR = semget(semReadKey, 1, 0666 | IPC_CREAT);
    semW = semget(semWriteKey, 1, 0666 | IPC_CREAT);

    // Validate on sem
    if (semR == -1 || semW == -1)
        perror("Error in creating semaphore\n");

    // Receiving the scheduling type
    down(semR);
    printf("Scheduler:\t\tStart reading the scheduler type\n");

    // Wait for the receiver to send the data to the shared memory
    schedulerType = (SCH_TYPE) atoi(argv[1]);

    if (schedulerType == RR){
        memtype = atoi(argv[2]);
        Quantum = atoi(argv[3]);
    }else
        memtype = atoi(argv[2]);

    up(semW);
    // Initialization of the Clock
    initClk();
    

    // Previous Clock Initialization for Sch. Algorithm
    int prevClk = getClk();

    // Initializing the file (Set the template) for scheduler.log
    f = fopen("../io/scheduler.log", "w");
    fprintf(f, "%s", "#At time x process y state arr w total z remain y wait k\n");
    fclose(f);

    // Initializing the file (Set the template) for memory.log
    f = fopen("../io/memory.log", "w");
    fprintf(f, "%s", "#At time x allocated y bytes for process z from i to j\n");
    fclose(f);

    // Previous Process Initialization before Main Loop
    prev_process = NULL;
    // Scheduler Main Loop
    while (true)
    {
        // Check the waiting list to find if a process can be allocated in memory
        int clock = getClk();
        // Check/Proceed in reading coming processes
        readProcesses(-1);
        // ========================================================
        // Waiting Processes Management Block =====================
        // ========================================================
        // ==== Verify that this block runs once per second
        // ========================================================
        if (clock > pclk && queue_size(waitingList) > 0)
        {
            Queue* waitingListCopy = waitingList; // Waiting List Pointer Copy
            
            // Current Number of Waiting Processes
            int waitingListSize = queue_size(waitingListCopy);

            // Looping on the waiting processes
            for (int i = 0; i < waitingListSize; i++)
            {
                // Queue top to check if the process can be forked and allocated
                pcb *process = queue_top(waitingListCopy);
                queue_pop(waitingListCopy);

                /*
                    Checks for Memory Allocation
                    Based on Memory Type
                    memtype == 1 -> First Fit Algorithm
                    memtype == 2 -> Buddy Algorithm
                */
                int memIdx;
                if(memtype == 1){
                    memIdx = allocateProcessMemory(mem,process->memsize);
                }else{
                    memIdx = allocateBuddyProcessMemory(mem,process->memsize);
                }

                //  In case that process can be allocated
                if(memIdx != -1){
                    /*
                        - Update the Prev Clock
                        - Update Process Memory Index 
                        - Fork the Process
                    */
                    pclk = clock;
                    process->memidx = memIdx;
                    
                    f = fopen("../io/memory.log", "a+");
                    fprintf(f, "At time %d %s %d from process %d from %d to %d\n",
                    getClk(), 
                    "allocated", process->memsize,process->id, process->memidx,
                    ((memtype == 1) ? process->memidx+process->memsize:process->memidx+roundToPowerOf2(process->memsize)) - 1);
                    fclose(f);
                    
                    // Create a child process to run the process
                    int pfid = fork();
                    if (pfid == -1)
                    {
                        perror("Error in instantiating Process");
                    }
                    else if (pfid == 0)
                    {
                        // Create the child process with the given arguments
                        char arg1[100], arg2[100];
                        sprintf(arg1, "%d", process->id);
                        sprintf(arg2, "%d", process->runtime);
                        int exec_proc = execl("./process.out", "process.out", arg1, arg2, (char *)0);
                    }
                    
                    // Save the child process ID in the process control block
                    process->pid = pfid;    

                    // Stop the child process from running
                    kill(process->pid, SIGSTOP);                

                    // Enqueue the process in the appropriate ready queue based on the scheduling algorithm
                    if (schedulerType == SRTN)
                    {
                        enqueue(p_q, process, process->runtime);
                    }
                    if (schedulerType == HPF)
                    {
                        enqueue(p_q, process, process->priority);
                    }
                    if (schedulerType == RR)
                    {
                        queue_push(RRQ, (void *)process);
                    }
                }
                else
                {
                    queue_push(waitingListCopy,process);
                }
            }
        }
        // Check if the scheduler type is Shortest Remaining Time Next (SRTN)
        if (schedulerType == SRTN)
        {
            // Get the current system clock time
            int clk = getClk(); // 2
            // Check if there are any processes in the ready queue
            if (getCount(p_q) > 0)
            {
                // Get the process with data_structures/the shortest remaining time (SRTN)
                current_process = peek(p_q); 
                if(!current_process) continue;
                // Check if the current process is not already running and has remaining time
                if (current_process->state != RUNNING && current_process->state != FINISHED && current_process->remainingtime > 0)
                {
                    // If the current process was in the ready state, set its start time to the current system clock time
                    if (current_process->state == READY)
                        current_process->starttime = clk;
                    // Send a signal to the current process to continue running
                    kill(current_process->pid, SIGCONT);
                    // Check if there was a previously running process that was preempted
                    if (prev_process && prev_process->remainingtime > 0 && prev_process->state == RUNNING)
                    {
                        // Set the previous process to the blocked state
                        prev_process->state = BLOCKED;
                        // Append a message to the scheduler log file to indicate that the previous process was stopped
                        kill(prev_process->pid, SIGSTOP);
                        
                        prev_process->remainingtime = temp_remaining - getClk() + temp_start;
                        setSpecificKeyValue(p_q, prev_process->id, prev_process->remainingtime);

                        f = fopen("../io/scheduler.log", "a+");
                        fprintf(f, "At time %d process %d %s arr %d total %d remain %d wait %d\n",
                                getClk(), prev_process->id,
                                "stopped", prev_process->arrival, prev_process->runtime, prev_process->remainingtime,
                                prev_process->starttime - prev_process->arrival);
                        // Send a signal to the previous process to stop running
                        fclose(f);

                    }
                    // Append a message to the scheduler log file to indicate that the current process was started or resumed
                    f = fopen("../io/scheduler.log", "a+");
                    fprintf(f, "At time %d process %d %s arr %d total %d remain %d wait %d\n",
                            getClk(), current_process->id,
                            (current_process->state == READY) ? "started" : "resumed",
                            current_process->arrival, current_process->runtime, current_process->remainingtime,
                            current_process->starttime - current_process->arrival);
                    fclose(f);

                    // Set the state of the current process to running
                    current_process->state = RUNNING;

                    temp_start = clk;
                    temp_remaining = current_process->remainingtime;
                    // Set the previous process to the current process
                    prev_process = current_process;
                }else
                // If the current process is already running, decrement its remaining time by one
                // and decrease its priority in the ready queue
                if (clk > prevClk && flag == 0)//
                {
                    prevClk = clk;
                    current_process->remainingtime--;
                    decreaseKey(p_q);
                    flag = 1;

                }else if(flag == 1 && clk < getClk()){
                    flag = 0;
                }
            }
            else
            {
                // If there are no processes in the ready queue, set the previous clock time to the current system clock time
                if(flag == 1 && clk < getClk()){
                    flag = 0;
                }
                else if(clk == getClk())
                    prevClk = clk;

            }
        }

        else if (schedulerType == RR)
        {
            // Clock Instance
            int clk = getClk();

            // Update the Current Process Remaining Time
            if (clk > prevClk)
            {
                if (current_process && !queue_is_empty(RRQ))
                {
                    current_process->remainingtime--;
                }
                prevClk = clk;
            }

            // Context Switch Between Processes
            if (clk - startRunTime >= Quantum && !queue_is_empty(RRQ) && current_process && current_process->remainingtime > 0)
            {
                if (queue_size(RRQ) == 1)
                {
                    startRunTime = clk; // reset the quantum - don't preempt
                }
                else
                {
                    kill(current_process->pid, SIGUSR2);
                    f = fopen("../io/scheduler.log", "a+");
                    fprintf(f, "At time %d process %d %s arr %d total %d remain %d wait %d\n",
                            getClk(), current_process->id, "stopped", current_process->arrival, current_process->runtime,
                            current_process->remainingtime, current_process->starttime - current_process->arrival);
                    fclose(f);
                    current_process->state = READY;
                    queue_pop(RRQ);
                    queue_push(RRQ, current_process);
                }
            }

            // Manage Current Process Start
            if (!queue_is_empty(RRQ))
            {
                current_process = (pcb *)queue_top(RRQ);

                if (current_process->state != RUNNING && current_process->remainingtime > 0)
                {
                    startRunTime = getClk();
                    kill(current_process->pid, SIGCONT);
                    current_process->starttime = getClk();
                    f = fopen("../io/scheduler.log", "a+");
                    fprintf(f, "At time %d process %d %s arr %d total %d remain %d wait %d\n",
                            getClk(), current_process->id,
                            (current_process->state == READY) ? "started" : "resumed",
                            current_process->arrival, current_process->runtime, current_process->remainingtime,
                            current_process->starttime - current_process->arrival);
                    fclose(f);
                    current_process->state = RUNNING;
                    if (prev_process)
                    {
                        prev_process->remainingtime = temp_remaining - getClk() + temp_start;   // 6
                        prev_process->state = BLOCKED;
                    }

                    temp_start = clk;
                    temp_remaining = current_process->remainingtime;
                   
                    prev_process = current_process;
                }
            }
        }
        else if (schedulerType == HPF)
        {
            // Get the current clock cycle
            int clk = getClk();

            // If there are processes in the queue
            if (getCount(p_q) > 0)
            {
                // Get the process at the head of the queue
                current_process = peek(p_q);
                if(!current_process) continue;
                if (current_process->state != RUNNING && current_process->remainingtime > 0)
                {
                    // If the process is in the READY state
                    // Set the start time of the process to the current time
                    current_process->starttime = clk;
                    // Send a SIGCONT signal to the process
                    kill(current_process->pid, SIGCONT);
                    
                    // Open the scheduler log file for appending
                    f = fopen("../io/scheduler.log", "a+");
                    // Write a log message to the scheduler log file
                    fprintf(f, "At time %d process %d %s arr %d total %d remain %d wait %d\n",
                            getClk(), current_process->id,
                            (current_process->state == READY) ? "started" : "resumed",
                            current_process->arrival, current_process->runtime, current_process->remainingtime,
                            current_process->starttime - current_process->arrival);
                    // Close the scheduler log file
                    fclose(f);
                    // Set the state of the process to RUNNING
                    current_process->state = RUNNING;

                    temp_start = clk;
                    temp_remaining = current_process->remainingtime;
                    // Decrease the priority of the process in the queue
                    decreaseKeyValue(p_q, -1);
                }
                else
                {
                    // If the process is in the RUNNING state
                    // If the current clock cycle is greater than the previous clock cycle
                    if (clk > prevClk)
                    {
                        // Decrement the remaining time of the process
                        current_process->remainingtime--;
                        // Set the previous clock cycle to the current time
                        prevClk = clk;
                    }
                }
            }
        }
    
        bool end_scheduler = 0;
        switch(schedulerType)
        {
            case RR: if (no_more_processes && queue_size(RRQ) == 0 && queue_size(waitingList) == 0) end_scheduler = true; break;
            case SRTN: case HPF: if (no_more_processes && getCount(p_q) == 0 && queue_size(waitingList) == 0) end_scheduler = true; break;
        }

        if (end_scheduler) break;
    

    }
    printf("scheduler finished\n");
    releaseResources();
    kill(getppid(),SIGUSR1);

}
