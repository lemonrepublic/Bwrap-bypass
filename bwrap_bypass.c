#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dlfcn.h>
#include <string.h>

static int (*real_execve)(const char *filename, char *const argv[], char *const envp[]) = NULL;

int execve(const char *filename, char *const argv[], char *const envp[]) {
    real_execve = dlsym(RTLD_NEXT, "execve");

    // Intercept invocations targeting bubblewrap
    if (strstr(filename, "bwrap") != NULL) {
        int i = 0;
        while (argv[i] != NULL) {
            if (strcmp(argv[i], "--") == 0 && argv[i + 1] != NULL) {
                // Redirect execution straight to the inner target binary
                return real_execve(argv[i + 1], &argv[i + 1], envp);
            }
            i++;
        }
    }
    return real_execve(filename, argv, envp);
}
