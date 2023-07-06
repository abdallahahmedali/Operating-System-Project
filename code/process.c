#include "headers.h"

// Global variables for the process state, remaining time, last processed time, and termination flag
int remainingtime;
int lastProcessedTime = 0;
int terminate = 0;
PROC_STATE state;
int prevClk;

// Handler function for the SIGCONT signal
void handler(int signum){
    prevClk = getClk();
}

// Handler function for the SIGUSR2 signal
void RRhandler(int signum){
    // Set the last processed time to the current time and set the termination flag to true
    lastProcessedTime = getClk();
    terminate = 1;
}

int main(int agrc, char * argv[])
{
    // Register the signal handlers for SIGCONT and SIGUSR2
    signal(SIGCONT, handler);
    signal(SIGUSR2, RRhandler);

    // Get the process ID and remaining time from the command-line arguments
    int id = atoi(argv[1]); // should be changed to the id in file
    remainingtime = atoi(argv[2]);

    // Initialize the clock and set the process state to READY
    initClk();
    state = READY;

    // Set the previous clock cycle to the current time to let the process start in the next clock after recieving the SIGCONT signal
    prevClk = getClk(); 

    // Loop until the remaining time for the process is zero
    while (remainingtime > 0)
    {
        // Get the current clock cycle
        int clk = getClk();
        
        // If the current clock cycle is greater than the previous clock cycle, decrement the remaining time and print a message
        if(clk > prevClk){
            remainingtime--;
            printf("Process:\t\tProc<%d> @ clk<%d>: current runtime = %d\n", id, clk, remainingtime);
            prevClk = clk;
        }
        
        // If a stop request was recieved from the scheduler, block the process
        if(terminate && prevClk == lastProcessedTime && remainingtime > 0){
            terminate = 0;
            kill(getpid(), SIGSTOP);
            prevClk = getClk();
        }    
    }

    // Print a message indicating that the process has terminated and send a SIGUSR2 signal to the parent process
    kill(getppid(), SIGUSR2);

    // Clean up the clock resources
    destroyClk(false);
    
    return 0;
}
