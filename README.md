# Watchdog Program

## Overview

This is a watchdog program written in C for a Linux machine that utilizes signals SIGUSR1 and SIGUSR2 for activation and termination, respectively. The watchdog is designed to monitor the status of another program and ensure its continuous execution.

## Features

- **Activation and Termination:**
  - Activate the watchdog using the function `MakeMeImmortal(size_t n, unsigned int interval, char *wd_path, char **argv);`.
  - Terminate the watchdog using the function `DoNotResuscitate(void);`.

- **Monitoring:**
  - Monitors the status of another program specified through command-line arguments.
  - If the monitored program does not send a periodic signal within a given interval, the watchdog takes corrective actions.

## Example:
```c
#include "watchdog.h" 

int main(int argc, char **argv) {
    // Activate the watchdog with parameters
    status_ty status = MakeMeImmortal(3, 10, "/path/to/watchdog", argv);

    if (status == ACTIVATION_SUCCESS) {
        // Your program logic here

        // Terminate the watchdog when your program finishes
        DoNotResuscitate();
    } else {
        // Handle activation failure
        fprintf(stderr, "Watchdog activation failed.\n");
    }

    return 0;
}

```

## Build and Test:
Compile your watchdog program using your preferred compiler. Additionally, 
a test file named usr_wd_test.c is provided for testing purposes
### Build
-Wl,-rpath=./watchdog_files/ -ansi -pedantic-errors -Wall -Wextra -DNDEBUG -O3 -o test_watchdog usr_wd_test.c ./watchdog_files/watchdog.a ./watchdog_files/lib_release_ds.so -Iwatchdog_files -ldl

### Run
./test_watchdog

