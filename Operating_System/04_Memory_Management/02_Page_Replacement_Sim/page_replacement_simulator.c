#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static int read_input(const char *path, int *frames, int refs[], int *nref) {
    FILE *fp = fopen(path, "r");
    if (!fp) {
        return -1;
    }
    if (fscanf(fp, "%d", frames) != 1) { 
        fclose(fp); return -2; 
    }
    int x, cnt = 0;
    while (fscanf(fp, "%d", &x) == 1) {
        if (cnt < 100000) {
            refs[cnt++] = x;
        }
    }
    fclose(fp);
    *nref = cnt;
    return 0;
}

static int in_frames(int frames[], int n, int page) {
    for (int i = 0; i < n; i++)
        if (frames[i] == page) {
            return i;
        }
    return -1;
}


static int simulate_optimal(int nframe, const int refs[], int nref) {
    int frames[1000]; for (int i=0;i<nframe;i++) frames[i] = -1;
    int faults = 0, filled = 0;

    for (int i = 0; i < nref; i++) {
        int p = refs[i];
        int pos = in_frames(frames, nframe, p);
        if (pos >= 0) continue;  // hit

        faults++; // miss
        if (filled < nframe) { 
            frames[filled++] = p;
            continue;
        }

        int victim = 0, far = -1;
        for (int f = 0; f < nframe; f++) {
            int j;
            for (j = i + 1; j < nref; j++)
                if (refs[j] == frames[f]) break;
            if (j == nref) { 
                victim = f; break;
            }  
            if (j > far) { 
                far = j; victim = f; 
            }
        }
        frames[victim] = p;
    }
    return faults;
}

//FIFO 
static int simulate_fifo(int nframe, const int refs[], int nref) {
    int frames[1000]; for (int i=0;i<nframe;i++) frames[i] = -1;
    int faults = 0, next = 0, filled = 0;

    for (int i = 0; i < nref; i++) {
        int p = refs[i];
        if (in_frames(frames, nframe, p) >= 0) continue; // hit

        faults++; // miss
        if (filled < nframe) {
            frames[filled++] = p;
        }
        else { 
            frames[next] = p; next = (next + 1) % nframe; 
        }
    }
    return faults;
}

// LRU: last used index 가장 작은 프레임 교체
static int simulate_lru(int nframe, const int refs[], int nref) {
    int frames[1000], last_used[1000];
    for (int i=0;i<nframe;i++){ frames[i] = -1; last_used[i] = -1; }

    int faults = 0, filled = 0;

    for (int i = 0; i < nref; i++) {
        int p = refs[i];
        int pos = in_frames(frames, nframe, p);
        if (pos >= 0) { 
            last_used[pos] = i; continue; 
        } // hit

        faults++; // miss
        if (filled < nframe) {
            frames[filled] = p; last_used[filled] = i; filled++;
        } else {
            int victim = 0;
            for (int f = 1; f < nframe; f++)
                if (last_used[f] < last_used[victim]){ 
                    victim = f;
                }
            frames[victim] = p; last_used[victim] = i;
        }
    }
    return faults;
}


static int simulate_clock(int nframe, const int refs[], int nref) {
    int frame[1000], refb[1000];
    for (int i=0;i<nframe;i++){ frame[i]=-1; refb[i]=0; }

    int faults = 0, hand = 0;

    for (int i = 0; i < nref; i++) {
        int p = refs[i];
        int pos = in_frames(frame, nframe, p);

        if (pos >= 0) {
            // hit: ref=1, hand 고정
            refb[pos] = 1;
            continue;
        }

        // miss: hand를 움직이며 ref=0 슬롯 찾음
        faults++;
        while (1) {
            if (frame[hand] == -1) {
                // 비어 있으면 바로 사용
                frame[hand] = p; refb[hand] = 1;
                hand = (hand + 1) % nframe; // fault 처리 후 hand 이동
                break;
            }
            if (refb[hand] == 0) {
                frame[hand] = p; refb[hand] = 1;
                hand = (hand + 1) % nframe;
                break;
            } else {
                // 두 번째 기회 부여
                refb[hand] = 0;
                hand = (hand + 1) % nframe;
            }
        }
    }
    return faults;
}

static void print_result(const char *name, int faults, int nref) {
    double rate = (nref > 0) ? (100.0 * faults / (double)nref) : 0.0;
    printf("%s:\n", name);
    printf("Number of Page Faults: %d\n", faults);
    printf("Page Fault Rate: %.2f%%\n\n", rate);
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s inputfile\n", argv[0]);
        return 1;
    }

    int nframe, refs[100000], nref;
    if (read_input(argv[1], &nframe, refs, &nref) != 0) {
        fprintf(stderr, "Failed to read input file: %s\n", argv[1]);
        return 1;
    }
    if (nframe <= 0 || nframe > 1000) {
        fprintf(stderr, "Invalid frame count: %d\n", nframe);
        return 1;
    }

    int f_opt  = simulate_optimal(nframe, refs, nref);
    int f_fifo = simulate_fifo(nframe, refs, nref);
    int f_lru  = simulate_lru(nframe, refs, nref);
    int f_clk  = simulate_clock(nframe, refs, nref);

    print_result("Optimal Algorithm", f_opt, nref);
    print_result("FIFO Algorithm",    f_fifo, nref);
    print_result("LRU Algorithm",     f_lru, nref);
    print_result("Clock Algorithm",   f_clk, nref);

    return 0;
}
