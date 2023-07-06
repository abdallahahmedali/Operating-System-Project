#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define null 0

struct processData
{
    int arrivaltime;
    int priority;
    int runningtime;
    int id;
    int memsize;
};

int main(int argc, char * argv[])
{
    FILE * pFile;
    pFile = fopen("../io/processes.txt", "w");
    int yes;
    struct processData pData;
    printf("Please enter the number of processes you want to generate: ");
    scanf("%d", &yes);
    srand(time(null));
    //fprintf(pFile,"%d\n",yes);
    fprintf(pFile, "#id arrival runtime priority memsize\n");
    pData.arrivaltime = 1;
    for (int i = 1 ; i <= yes ; i++)
    {
        //generate Data Randomly
        //[min-max] = rand() % (max_number + 1 - minimum_number) + minimum_number
        pData.id = i;
        pData.arrivaltime += rand() % (6); //processes arrives in order
        pData.runningtime = rand() % (5);
        pData.priority = rand() % (11);
        pData.memsize = rand() % (256);
        fprintf(pFile, "%d\t%d\t%d\t%d\t%d\n", pData.id, pData.arrivaltime, pData.runningtime, pData.priority,pData.memsize);
    }
    fclose(pFile);
}
