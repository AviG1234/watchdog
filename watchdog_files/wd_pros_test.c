#define _POSIX_C_SOURCE 200112L /* setenv */
#include <stdlib.h> /* setenv */
#include <sys/types.h>  /* getpid */
#include <unistd.h>  /* getpid */ 
#include <stdio.h>  /* sprintf */
#include <string.h> /* strncmp, strtok */ 
#include "utils.h"  /* RETURN_IF_BAD */

#include "watchdog_imp.h"

#define BUFF_SIZE 10

static int SetArgs(int argc, char *argv[], mmi_imp_args_ty *args);
static int SetWD_PID(int pid);

int main(int argc, char *argv[])
{
    mmi_imp_args_ty args = {0};
    
    printf("\nWD pross start\n");

    /* set WD_PID to getpid() */
    RETURN_IF_BAD((0 != SetWD_PID(getpid())), "SetWD_PID", 1);

    RETURN_IF_BAD(0 != SetArgs(argc, argv, &args), "SetArgs", 1); 

    MakeMeImmortalImp(&args);
    
    return 0;
}

static int SetArgs(int argc, char *argv[], mmi_imp_args_ty *args)
{
    char *token = NULL;

    args->wd_path = NULL;
    args->usr_path = argv[1];
    
    /* find max_signal_cnt_value and interval from argv" */
    token = strtok(argv[0], "_");
    RETURN_IF_BAD((NULL == token), "strtok", 1);
    args->max_signal_cnt_value = atoi(token);

    token = strtok(NULL, "_");
    RETURN_IF_BAD((NULL == token), "strtok", 1);
    args->interval = atoi(token);
 
    args->argv = (argv + 1);
    UNUSED(argc);

    return 0;
}

static int SetWD_PID(int pid)
{
    char *name = "WD_PID";
    char val[BUFF_SIZE] = {0};

    sprintf(val, "%d", pid);
    
    RETURN_IF_BAD((0 != setenv(name, val, 1)), "setenv",1);

    return 0;
}