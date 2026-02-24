
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>
#include <string.h>

#ifndef MAX_PROCESSES
#define MAX_PROCESSES 64  
#endif

static int read_pair_sum_by_index(const char *path, int idx, long *out_sum) {
    FILE *f = fopen(path, "r");
    if (!f) return -1;

    char buf[64];
    long a = 0, b = 0;
    int want1 = 2*idx;     
    int want2 = 2*idx + 1;

    for (int line = 0; ; line++) {
        if (!fgets(buf, sizeof(buf), f)) {
            fclose(f);
            errno = EIO;
            return -1; 
        }
        if (line == want1) {
            a = strtol(buf, NULL, 10);
        }
        if (line == want2) {
            b = strtol(buf, NULL, 10);
            break;
        }
    }
    fclose(f);
    *out_sum = a + b;  
    return 0;
}

static double elapsed_sec(struct timespec s, struct timespec e) {
    long sec  = e.tv_sec - s.tv_sec;
    long nsec = e.tv_nsec - s.tv_nsec;
    if (nsec < 0) { sec--; nsec += 1000000000L; }
    return (double)sec + (double)nsec / 1e9;
}

int main(void) {
    const char *path = "./temp.txt";

    struct timespec ts1, ts2;
    clock_gettime(CLOCK_MONOTONIC, &ts1);

    pid_t pids[MAX_PROCESSES];

    for (int i = 0; i < MAX_PROCESSES; i++) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            break;
        }
        if (pid == 0) {
            // Child
            long part = 0;
            if (read_pair_sum_by_index(path, i, &part) != 0) {
                _exit(0);
            }
            if (part < 0) part = 0;
            if (part > 255) part = 255;
            _exit((unsigned char)part);
        } else {
            // Parent
            pids[i] = pid;
        }
    }

    long total = 0;
    for (int i = 0; i < MAX_PROCESSES; i++) {
        int status = 0;
        pid_t w = wait(&status);
        if (w == -1) {
            perror("wait");
            continue;
        }
        if (WIFEXITED(status)) {
            total += WEXITSTATUS(status);
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &ts2);
    double sec = elapsed_sec(ts1, ts2);

    printf("value of fork : %ld\n", total);
    printf("%.6f\n", sec);
    return 0;
}
