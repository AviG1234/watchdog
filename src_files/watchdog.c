
#define _XOPEN_SOURCE 500  /* strdup */
#define _POSIX_C_SOURCE 200112L /* setenv */

#include <stdlib.h> /* setenv */
#include <semaphore.h>  /* sem_open, sem_post, sem_wait */
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <pthread.h>    /* pthread_t, pthread_create */
#include <stdio.h>  /* sprintf */
#include <string.h> /* strdup */
#include <errno.h>  /* errno */
#include <signal.h> /* kill, pthread_sigmask */
#include <sys/types.h>  /* getpid, getppid, kill */
#include <unistd.h>  /* getpid, getppid */
#include <assert.h> /* assert */


#include "watchdog_imp.h"    /* MakeMeImmortalImp */
#include "watchdog.h"

#define BUFF_SIZE 200
#define ADD_TO_ARGV 2

/* two err handling defines to niglect including my utils.h file */
#define RETURN_IF_BAD(is_bad_test, err_msg, ret_val) \
    if((is_bad_test)) { \
        fprintf(stderr, "%s; err line %d;  %s\n", __FILE__, __LINE__, err_msg); \
        return ret_val; \
    }

#define PRINT_IF_BAD(is_bad_test, err_msg, err_no) \
    if((is_bad_test)) { \
        if((0 != err_no)) { \
            fprintf(stderr, "%s; err line %d,  %s; errno num: %d, ", __FILE__,__LINE__, err_msg, err_no); \
            perror("Error msg: "); \
        }   \
        else {  \
            fprintf(stderr, "%s; err line %d;  %s\n", __FILE__,__LINE__, err_msg); \
        } \
    }

static int SetWD_PID(int pid);
static int GetWD_PID();
char *GetProsPath();
mmi_imp_args_ty *InitArgs(size_t n, unsigned int interval, char *wd_path, char **argv);

status_ty MakeMeImmortal(size_t n, unsigned int interval, char *wd_path, char **argv)
{

    pthread_t th = 0;
    sem_t *wd_sem;
    char sem_name[BUFF_SIZE] = {0};
    
    /*char *usr_path = NULL;*/
    char *str_fncs_res = NULL;
    /*int i = 0;
    char **argv_cp = NULL;*/
    sigset_t set;
    mmi_imp_args_ty *args = NULL;

    /* check WD_PID */
    if(-1 == GetWD_PID())
    {
        SetWD_PID(1);
    } 

    /* block sig1 ang sig2 */
    {
    RETURN_IF_BAD(0 != sigemptyset(&set), "sigemptyset", WD_FAILURE);
    RETURN_IF_BAD(0 != sigaddset(&set, SIGUSR1), "sigaddset", WD_FAILURE);
    RETURN_IF_BAD(0 != sigaddset(&set, SIGUSR2), "sigaddset", WD_FAILURE);
    RETURN_IF_BAD(0 != pthread_sigmask(SIG_BLOCK, &set, NULL), "pthread_sigmask", WD_FAILURE);
    }

    /* create sem */
    {
    RETURN_IF_BAD(NULL == (str_fncs_res =  strrchr(GetProsPath(),'/')), "strrchr", WD_FAILURE);
    RETURN_IF_BAD(0 == strcpy(sem_name, str_fncs_res + 1), "strrchr", WD_FAILURE);
    strcat(sem_name, ".semfile");
    RETURN_IF_BAD((SEM_FAILED == (wd_sem = sem_open(sem_name, O_CREAT, S_IRUSR | S_IWUSR, 0))), "sem_open", WD_FAILURE);
    }

    /* update args filds */
    args = InitArgs(n, interval, wd_path, argv);
    if(NULL == args)
    {
        PRINT_IF_BAD(0 != (sem_unlink(sem_name)), "sem_unlink", errno);
        return WD_FAILURE;
    }
  
    RETURN_IF_BAD((0 != (pthread_create(&th, NULL, MakeMeImmortalImp, args))), "pthread_create", WD_FAILURE);
    RETURN_IF_BAD((0 != (pthread_detach(th))), "pthread_detach", WD_FAILURE);

    return WD_SUCCESS;
}

void DoNotResuscitata(void)
{
    int target_pid = GetWD_PID();

    switch(target_pid)
    {
        case -1: 
            return;
        case 1: 
            PRINT_IF_BAD((0 != kill(target_pid, SIGUSR2)), "kill", errno);
            return;
        
        default:
            printf("Sending kill to %d\n", target_pid);
            PRINT_IF_BAD((0 != kill(target_pid, SIGUSR2)), "kill", errno);
            printf("Sending kill to me %d\n", getpid());
            PRINT_IF_BAD((0 != kill(getpid(), SIGUSR2)), "kill", errno);
    }
}

static int GetWD_PID()
{
    char *name = "WD_PID";
    char *val = NULL;
    int res = 0;

    if(NULL == (val = getenv(name)))
    {
        return -1;
    }

    res = atoi(val);
    
    if((0 == res) && ('0' != *val))
    {
        return -1;
    }

    return res;
}

static int SetWD_PID(int pid)
{
    char *name = "WD_PID";
    char val[BUFF_SIZE] = {0};

    sprintf(val, "%d", pid);
    
    RETURN_IF_BAD((0 != setenv(name, val, 1)), "setenv",1);

    return 0;
}

mmi_imp_args_ty *InitArgs(size_t n, unsigned int interval, char *wd_path, char **argv)
{
    size_t i = 0;
    char **argv_cp = NULL;
    char str_tmp[BUFF_SIZE] = {0};
    mmi_imp_args_ty *args = (mmi_imp_args_ty*)malloc(sizeof(mmi_imp_args_ty));
    RETURN_IF_BAD((NULL == args), "malloc", NULL);
    
    args->max_signal_cnt_value = n;
    args->interval = interval;
    args->wd_path = wd_path;

    args->usr_path = GetProsPath();
    if(NULL == args->usr_path)
    {
        PRINT_IF_BAD(1, "GetProsPath", errno);
        free(args);

        return NULL;
    }
    
    for ( i = 0; NULL != argv[i]; ++i){;}

    if(NULL == (argv_cp = (char **)malloc(sizeof(char*) * i + ADD_TO_ARGV)))
    {
        PRINT_IF_BAD(1, "malloc", errno);
        free(args);

        return NULL;
    }
    
    if(0 == sprintf(str_tmp, "%ld_%d", n, interval))
    {
        PRINT_IF_BAD(1, "sprintf", errno);
        free(args);
        free(argv_cp);

        return NULL;
    }

    if(NULL == (argv_cp[0] = strdup(str_tmp)))
    {
        PRINT_IF_BAD(1, "strdup", errno);
        free(args);
        free(argv_cp);
        
        return NULL;
    }

    for ( i = 0; NULL != argv[i]; ++i)
    {
        argv_cp[1 + i] = argv[i];
    }

    argv_cp[1 + i] = NULL;

    args->argv = argv_cp;

    return args;
}

char *GetProsPath()
{
    static char* usr_path = NULL;

    if(NULL == usr_path)
    {
        RETURN_IF_BAD(NULL == (usr_path = getenv("_")), "getenv", NULL);
        RETURN_IF_BAD(' ' == usr_path[0], "getenv", NULL);
    }

    return usr_path;
}