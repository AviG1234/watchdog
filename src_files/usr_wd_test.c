#define _POSIX_C_SOURCE 199309L /* clock_gettime */

#include <stdlib.h> /* system */
#include <stdio.h>  /* printf */
#include <time.h>	/* clock_gettime */
#include <unistd.h> /* sleep */

#include "watchdog.h"

void PrintArgv(char *argv[]);
int PrintOneSecGap(int reps);

int main(int argc, char *argv[])
{
    size_t trys = 5;
    unsigned int interval = 2;
    char *wd_path = "./watchdog_files/wd_process";
    size_t i = 0;

    int reps = 15;

    printf("new process created\n");
    PrintArgv(argv);

    if(WD_SUCCESS == MakeMeImmortal(trys, interval,wd_path, argv))
    {        
       PrintOneSecGap(reps);
    }
    else
    {
        printf("MakeMeImmortal faild\n");
    }


    printf("DoNotResuscitata\n");
    DoNotResuscitata();
    PrintOneSecGap( reps);
    printf("end\n");


    return 0;
}

void PrintArgv(char *argv[])
{
    int i = 0;

    while (NULL != argv[i])
    {
        printf("argv[%d]: %s\n",i, argv[i]);
        ++i;
    }

    printf("\n");
}

int PrintOneSecGap(int reps)
{
    int i = 0;

    int sleep_time = 0;

    for ( i = 0; i < reps; i++)
    {
        sleep_time = 5;
        while (0 < (sleep_time = sleep(sleep_time)))
        {
            ;
        }

        printf("REP: %d\n",i);
    }
    
    return 0;
}
