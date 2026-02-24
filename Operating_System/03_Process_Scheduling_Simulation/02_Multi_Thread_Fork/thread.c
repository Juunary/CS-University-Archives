#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>

#ifndef MAX_PROCESSES
#define MAX_PROCESSES 64  
#endif

typedef struct {
    int idx;             
    const char *path;     
    long result;          
} worker_arg_t;

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
        if (line == want1) a = strtol(buf, NULL, 10);
        if (line == want2) { b = strtol(buf, NULL, 10); break; }
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

static void *worker(void *argp) {
    worker_arg_t *arg = (worker_arg_t *)argp;
    long part = 0;
    if (read_pair_sum_by_index(arg->path, arg->idx, &part) == 0) {
        arg->result = part;
    } else {
        arg->result = 0;
    }
    return NULL;
}

int main(void) {
    const char *path = "./temp.txt";

    pthread_t th[MAX_PROCESSES];
    worker_arg_t args[MAX_PROCESSES];

    struct timespec ts1, ts2;
    clock_gettime(CLOCK_MONOTONIC, &ts1);

    for (int i = 0; i < MAX_PROCESSES; i++) {
        args[i].idx = i;
        args[i].path = path;
        args[i].result = 0;
        if (pthread_create(&th[i], NULL, worker, &args[i]) != 0) {
            perror("pthread_create");
        }
    }

    long total = 0;
    for (int i = 0; i < MAX_PROCESSES; i++) {
        (void)pthread_join(th[i], NULL);
        total += args[i].result;
    }

    clock_gettime(CLOCK_MONOTONIC, &ts2);
    double sec = elapsed_sec(ts1, ts2);

    printf("value of thread : %ld\n", total);
    printf("%.6f\n", sec);
    return 0;
}
