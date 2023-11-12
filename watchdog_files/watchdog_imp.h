/*******************************************************************************
 * Author: HRD29
 * Reviewer: Naama 
 * Description: watchdog API
 * Version: 0.1
 * general notes: the module use the SIGUSR1 and SIGUSR2 signals, thay canot be overwritten or mask.
 *                Compile and link with -pthread
*******************************************************************************/

#ifndef __WATCHDOG_IMP__
#define __WATCHDOG_IMP__

#include <stddef.h>	/* size_t */


typedef struct mmi_imp_args
{
    char *wd_path;
    char *usr_path; 
    char **argv;
    size_t max_signal_cnt_value;
    unsigned int interval;

} mmi_imp_args_ty;

/*******************************************************************************
*  Description:	    create watchdog process to reactivate current process in case of termination
*  Parameters:   n: number of received signals before reactivation process.
*                interval: time (sec) interval between sent and received signals
*                   #   for example if n is 5 and interval is 3 the calling process will be 
*                       reactivated after 15 seconds without watchdog receiving signal from process
*               wd_path: relative path to the file WatchDogProcess.out from compilation folder.
                argv: cmd argv arguments recived in main function
* 
*  Return value: 	SUCCESS, else returns FAILURE.
*******************************************************************************/
void* MakeMeImmortalImp(void *args);


#endif /*__WATCHDOG_IMP__*/
