#include "headers.h"

int semR,semW,shmid;
void clearResources(int);

int readProcesses(char*);
int terminate(int signum)
{
    clearResources(signum);
    exit(0);
}

int main(int argc, char *argv[])
{
    signal(SIGINT, clearResources);
    signal(SIGUSR1,terminate);

    key_t shm_key = ftok("../code/shmfile", 65);
    // 4 bytes --> Type of message (Set Scheduler Type, Send Process)
    // 4 bytes --> OPT: Process ID
    // 8 bytes --> Data (SCH_TYPE type|| pcb* process ptr)
    shmid = shmget(shm_key, sizeof(int) * 2 + sizeof(pcb), IPC_CREAT | 0666);
    void *schshmaddr = shmat(shmid, (void *)0, 0);

    key_t semReadKey = ftok("../code/ReadFile", 65);
    key_t semWriteKey = ftok("../code/WriteFile", 65);
    semR = semget(semReadKey, 1, 0666 | IPC_CREAT);
    semW = semget(semWriteKey, 1, 0666 | IPC_CREAT);
    if (semR == -1 || semW == -1)
        perror("Error in creating semaphore");

    semun.val = 0; /* initial value of the semaphore, Binary semaphore */
    if (semctl(semR, 0, SETVAL, semun) == -1)
        perror("Error in semctl");
    if (semctl(semW, 0, SETVAL, semun) == -1)
        perror("Error in sem");

    // TODO Initialization
    // 1. Read the input files.
    int n = readProcesses(argv[1]);

    for (int i = 0; i < n; i++)
        printf("%d\t%d\t%d\t%d\t%d\n",
               processes_array[i].id,
               processes_array[i].arrival,
               processes_array[i].runtime,
               processes_array[i].priority,
               processes_array[i].memsize);
    // Clock Process Fork
    int clk_exec_id = fork();
    if (clk_exec_id == -1)
        perror("Error in clk forking");
    else if (clk_exec_id == 0)
        execl("./clk.out", "clk.out", (char *)0);

    char q[10];
    char s[10];
    char sc[10];
    SCH_TYPE sch;
    for (int i = 2; i < argc; i+=2)
    {
        if(strcmp(argv[i],"-sch") == 0){
            strcpy(sc, argv[i + 1]);
            sch = atoi(sc);
        }else if(strcmp(argv[i],"-q") == 0){
            strcpy(q, argv[i + 1]);
        }else if(strcmp(argv[i],"-mem") == 0){
            strcpy(s, argv[i + 1]);
        }
    }
    

    int sch_exec_pid = fork();
    if (sch_exec_pid == -1)
        perror("Error in sch forking");
    else if (sch_exec_pid == 0){
        if (sch != RR){
            execl("./scheduler.out", "scheduler.out",sc,s,(char *)0);
        }
        else
        {
            execl("./scheduler.out", "scheduler.out",sc,s,q, (char *)0);
        }
    }

    up(semR);
    // Verify that scheduler read from the shared memory
    down(semW);

    // initiating clock
    initClk();
    printf("Process Generator:\tClock initialized\n");

    int current_process_index = 0;

    int prevClk = -1;

    while (true)
    {
        // sch duties
        int cur_clk = getClk();

        if (cur_clk <= prevClk)
        {
            continue;
        }

        prevClk = cur_clk;

        int count = 0;
        while (current_process_index < n && processes_array[current_process_index].arrival <= cur_clk)
        {
            count++;
            current_process_index++;
        }
        if (current_process_index == n && count == 0)
            count = -1;

        *((int *)schshmaddr + 1) = count; // Process id
        up(semR);
        down(semW);

        if (count == -1)
        {
            break;
        }

        for (int i = current_process_index - count; i < current_process_index; i++)
        {
            // Sending process
            *(int *)schshmaddr = SR_PROC_MSG;                     // Process message type
            *((int *)schshmaddr + 1) = processes_array[i].id;     // Process id
            *(pcb *)((int *)schshmaddr + 2) = processes_array[i]; // PCB
            up(semR);

            // Scheduler has read?
            down(semW);
        }
    }
    while(true);
    //clearResources(-1);
}

void clearResources(int signum)
{
    free(processes_array);
    semctl(semR, 0, IPC_RMID, 0);
    semctl(semW, 0, IPC_RMID, 0);
    shmctl(shmid, IPC_RMID, 0);
    destroyClk(true);
    printf("Process Generator:\tResources Cleared\n");
    exit(0);
}

int readProcesses(char* file)
{
    FILE *fp, *ftraverse;

    // Open the file in read mode
    fp = fopen(file, "r");
    ftraverse = fopen(file, "r");

    int num_lines = 0, num_process = 0;
    char buffer[255];

    // Check if the file exists and can be opened
    if (fp == NULL)
    {
        printf("File cannot be opened\n");
        return 1;
    }

    while (fgets(buffer, 255, fp) != NULL)
    {
        if(buffer[0] != '#') num_process++;
        num_lines++;
    }

    fclose(fp);

    // Open the file in read mode
    fp = fopen(file, "r");

    // Check if the file exists and can be opened
    if (fp == NULL)
    {
        printf("File cannot be opened\n");
        return 1;
    }
    if (ftraverse == NULL)
    {
        printf("File cannot be opened\n");
        return 1;
    }

    // reserve memory for the global process control block array
    processes_array = malloc((num_process) * sizeof(pcb));



    // Read integer data from the file using fscanf()
    int curr_p_num = 0;
    for (int i = 0; i < num_lines; i++)
    {
        if(fgets(buffer, 255, ftraverse) != NULL){
            if(buffer[0] == '#') {
                fgets(buffer, 255, fp);
                continue;
            }
            fscanf(fp, "%d\t%d\t%d\t%d\t%d\n", &(processes_array[curr_p_num].id), &(processes_array[curr_p_num].arrival), &(processes_array[curr_p_num].runtime), &(processes_array[curr_p_num].priority), &(processes_array[curr_p_num].memsize));
            processes_array[curr_p_num].remainingtime = processes_array[curr_p_num].runtime;
            curr_p_num++;
        }

    }
    
    // Close the file
    fclose(fp);
    fclose(ftraverse);

    return num_process;
}