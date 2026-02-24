#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>   
#include <ctype.h>

typedef struct {
    int pid;       
    int arr;       
    int burst;     
    int remain;    
    int start;     
    int finish;  
} Task;

typedef struct {
    Task *a;
    int n;
} TaskSet;

static void die(const char *m){ perror(m); exit(1); }

static TaskSet load_tasks(const char *path){
    FILE *f = fopen(path, "r");
    if(!f) die(path);
    Task *a = NULL; int n=0, cap=0;
    int p, ar, bu;
    while(fscanf(f, "%d %d %d", &p, &ar, &bu) == 3){
        if (bu < 0) { fprintf(stderr, "negative burst not allowed\n"); exit(1); }
        if(n == cap){ cap = cap ? cap*2 : 16; a = (Task*)realloc(a, sizeof(Task)*cap); }
        a[n].pid = p;
        a[n].arr = ar;
        a[n].burst = bu;
        a[n].remain = bu;
        a[n].start = -1;
        a[n].finish = -1;
        n++;
    }
    fclose(f);
    if(n == 0){
        fprintf(stderr, "no tasks in input\n");
        exit(1);
    }
    TaskSet ts = { a, n };
    return ts;
}


static void gantt_tick(char **buf, int *len, int pid)
{
    char tmp[32];
    int add = snprintf(tmp, sizeof(tmp), "| P%d ", pid); 

    char *newbuf = realloc(*buf, (size_t)(*len + add + 1)); 
    if (!newbuf) { 
        return;
    }
    *buf = newbuf;
    memcpy((*buf) + *len, tmp, (size_t)add);
    *len += add;
    (*buf)[*len] = '\0';  
}



#ifdef PRINT_IDLE
static void gantt_idle(char **buf, int *len){
    *buf = (char*)realloc(*buf, (size_t)((*len + 1) * 6));
    sprintf((*buf) + (*len) * 6, "| -- ");
    (*len)++;
}
#endif

typedef struct {
    double avg_wait;
    double avg_turn;
    double avg_resp;
    double cpu_util;
} Stats;

static Stats compute_stats(Task *t, int n, double busy_time, double total_time){
    double sum_wait = 0.0, sum_turn = 0.0, sum_resp = 0.0;
    for(int i=0;i<n;i++){
        int turnaround = t[i].finish - t[i].arr;
        int waiting    = turnaround - t[i].burst;
        int response   = t[i].start - t[i].arr;
        sum_turn += turnaround;
        sum_wait += waiting;
        sum_resp += response;
    }
    Stats s;
    s.avg_wait = sum_wait / n;
    s.avg_turn = sum_turn / n;
    s.avg_resp = sum_resp / n;
    s.cpu_util = (total_time > 0.0) ? (busy_time / total_time) * 100.0 : 0.0;
    return s;
}

static void reset_for_algo(TaskSet ts){
    for(int i=0;i<ts.n;i++){
        ts.a[i].remain = ts.a[i].burst;
        ts.a[i].start  = -1;
        ts.a[i].finish = -1;
    }
}

typedef struct { int *q; int f, r, cap; } Q;
static Q q_new(int cap){ Q q; q.q=(int*)malloc(sizeof(int)*cap); q.f=q.r=0; q.cap=cap; return q; }
static int q_empty(Q *q){ return q->f == q->r; }
static void q_push(Q *q, int v){
    if(q->r >= q->cap){ fprintf(stderr,"ready queue overflow (cap=%d)\n", q->cap); exit(1); }
    q->q[q->r++] = v;
}
static int  q_pop (Q *q){ return q->q[q->f++]; }

static Stats sim_fcfs(TaskSet ts){
    Task *t = ts.a; int n = ts.n;
    int time = 0, finished = 0;
    double busy = 0.0, total = 0.0;
    char *g = NULL; int glen = 0;
    int cur = -1;

    while(finished < n){
        int next = -1; int best_arr = 1e9; int best_pid = 1e9;
        for(int i=0;i<n;i++){
            if(t[i].remain > 0 && t[i].arr <= time){
                if(t[i].arr < best_arr || (t[i].arr==best_arr && t[i].pid < best_pid)){
                    best_arr = t[i].arr; best_pid = t[i].pid; next = i;
                }
            }
        }

        if(next < 0){
#ifdef PRINT_IDLE
            gantt_idle(&g, &glen);
#endif
            time++; total++;
            continue;
        }

        total += 0.1;

        cur = next;
        if(t[cur].start < 0) t[cur].start = time;

        int run = t[cur].remain;
        for(int k=0;k<run;k++){
            gantt_tick(&g, &glen, t[cur].pid);
            t[cur].remain--; time++; busy++; total++;
        }
        t[cur].finish = time;
        finished++;
        cur = -1;
    }

    Stats s = compute_stats(t, n, busy, total);
    printf("Gantt Chart:\n%s|\n", g ? g : "");
    free(g);
    return s;
}

static Stats sim_sjf(TaskSet ts){
    Task *t = ts.a; int n = ts.n;
    int time = 0, finished = 0;
    double busy = 0.0, total = 0.0;
    char *g = NULL; int glen = 0;
    int cur = -1;

    while(finished < n){
        int next = -1; int best_burst = 1e9; int best_arr = 1e9; int best_pid = 1e9;
        for(int i=0;i<n;i++){
            if(t[i].remain > 0 && t[i].arr <= time){
                if( (t[i].burst < best_burst) ||
                    (t[i].burst == best_burst && (t[i].arr < best_arr ||
                     (t[i].arr == best_arr && t[i].pid < best_pid))) ){
                    best_burst = t[i].burst; best_arr = t[i].arr; best_pid = t[i].pid; next = i;
                }
            }
        }

        if(next < 0){
#ifdef PRINT_IDLE
            gantt_idle(&g, &glen);
#endif
            time++; total++;
            continue;
        }

        total += 0.1; 
        cur = next;
        if(t[cur].start < 0) t[cur].start = time;

        int run = t[cur].remain;
        for(int k=0;k<run;k++){
            gantt_tick(&g, &glen, t[cur].pid);
            t[cur].remain--; time++; busy++; total++;
        }
        t[cur].finish = time;
        finished++;
        cur = -1;
    }

    Stats s = compute_stats(t, n, busy, total);
    printf("Gantt Chart:\n%s|\n", g ? g : "");
    free(g);
    return s;
}

static Stats sim_rr(TaskSet ts, int quantum){
    Task *t = ts.a; int n = ts.n;
    int time = 0, finished = 0;
    double busy = 0.0, total = 0.0;
    char *g = NULL; int glen = 0;

    Q rq = q_new(1000);       
    int inq[4096] = {0};       
    int cur = -1;

    while(finished < n){
        for(int i=0;i<n;i++){
            if(!inq[i] && t[i].remain > 0 && t[i].arr <= time){
                q_push(&rq, i);
                inq[i] = 1;
            }
        }

        if(cur == -1 && !q_empty(&rq)){
            int nxt = q_pop(&rq); inq[nxt] = 0;
            total += 0.1; 
            cur = nxt;
            if(t[cur].start < 0) t[cur].start = time;
        }

        if(cur == -1){
#ifdef PRINT_IDLE
            gantt_idle(&g, &glen);
#endif
            time++; total++;
            continue;
        }

        int slice = (t[cur].remain < quantum) ? t[cur].remain : quantum;
        for(int k=0;k<slice;k++){
            gantt_tick(&g, &glen, t[cur].pid);
            t[cur].remain--; time++; busy++; total++;

            for(int i=0;i<n;i++){
                if(!inq[i] && t[i].remain > 0 && t[i].arr <= time){
                    q_push(&rq, i);
                    inq[i] = 1;
                }
            }

            if(t[cur].remain == 0) break;
        }

        if(t[cur].remain == 0){
            t[cur].finish = time;
            finished++;
            cur = -1;
        }else{
            q_push(&rq, cur); inq[cur] = 1;
            cur = -1;
        }
    }

    free(rq.q);
    Stats s = compute_stats(t, n, busy, total);
    printf("Gantt Chart:\n%s|\n", g ? g : "");
    free(g);
    return s;
}

static Stats sim_srtf(TaskSet ts){
    Task *t = ts.a; int n = ts.n;
    int time = 0, finished = 0;
    double busy = 0.0, total = 0.0;
    char *g = NULL; int glen = 0;

    int cur = -1;

    while(finished < n){
        int next = -1; int best_rem = 1e9; int best_arr = 1e9; int best_pid = 1e9;
        for(int i=0;i<n;i++){
            if(t[i].remain > 0 && t[i].arr <= time){
                if( (t[i].remain < best_rem) ||
                    (t[i].remain == best_rem && (t[i].arr < best_arr ||
                     (t[i].arr == best_arr && t[i].pid < best_pid))) ){
                    best_rem = t[i].remain; best_arr = t[i].arr; best_pid = t[i].pid; next = i;
                }
            }
        }

        if(next < 0){
#ifdef PRINT_IDLE
            gantt_idle(&g, &glen);
#endif
            time++; total++;
            continue;
        }

        if(cur != next){
            total += 0.1;
            cur = next;
            if(t[cur].start < 0) t[cur].start = time;
        }

        gantt_tick(&g, &glen, t[cur].pid);
        t[cur].remain--; time++; busy++; total++;

        if(t[cur].remain == 0){
            t[cur].finish = time;
            finished++;
            cur = -1;
        }
    }

    Stats s = compute_stats(t, n, busy, total);
    printf("Gantt Chart:\n%s|\n", g ? g : "");
    free(g);
    return s;
}

int main(int argc, char **argv){
    if(argc < 3){
        fprintf(stderr,
            "Usage: %s input_file [FCFS|RR|SJF|SRTF] [time_quantum]\n"
            "  - RR requires time_quantum (in ms)\n", argv[0]);
        return 1;
    }

    TaskSet ts = load_tasks(argv[1]);
    reset_for_algo(ts);

    Stats s;
    if(!strcasecmp(argv[2], "FCFS")){
        s = sim_fcfs(ts);
    } else if(!strcasecmp(argv[2], "SJF")){
        s = sim_sjf(ts);
    } else if(!strcasecmp(argv[2], "RR")){
        if(argc < 4){
            fprintf(stderr, "RR needs time_quantum\n");
            free(ts.a);
            return 1;
        }
        int q = atoi(argv[3]);
        if(q <= 0) q = 1;
        s = sim_rr(ts, q);
    } else if(!strcasecmp(argv[2], "SRTF")){
        s = sim_srtf(ts);
    } else {
        fprintf(stderr, "Unknown algorithm: %s\n", argv[2]);
        free(ts.a);
        return 1;
    }

    printf("Average Waiting Time = %.2f\n", s.avg_wait);
    printf("Average Turnaround Time = %.2f\n", s.avg_turn);
    printf("Average Response Time = %.2f\n", s.avg_resp);
    printf("CPU Utilization = %.2f%%\n", s.cpu_util);

    free(ts.a);
    return 0;
}
