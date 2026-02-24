// SPDX-License-Identifier: MIT
// KWU OSLab A5-2 (2021202003) - FAT-like in-memory VFS with persistence
// CLI: create <name>, write <name> "<data>", read <name>, delete <name>, list
// On start: auto load; on exit: auto save.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define MAX_FILES   100
#define NUM_BLKS    1024
#define BLK_SZ      32

#define FAT_FREE    (-2)
#define FAT_END     (-1)

#define IMG_NAME    "kwufs.img"
#define NAME_MAXLEN 100

struct kwu_file {
    char name[NAME_MAXLEN + 1];
    int  size;      
    int  head;      
    int  used;      
};

static struct kwu_file g_dir[MAX_FILES];
static int            g_fat[NUM_BLKS];
static unsigned char  g_data[NUM_BLKS][BLK_SZ];

// 빈 블록 찾기
static int blk_find_free(void) {
    for (int i = 0; i < NUM_BLKS; i++)
        if (g_fat[i] == FAT_FREE) return i;
    return -1;
}
// 이름으로 디렉토리 항목 찾기
static int dir_find_by_name(const char *name) {
    for (int i = 0; i < MAX_FILES; i++)
        if (g_dir[i].used && strcmp(g_dir[i].name, name) == 0) return i;
    return -1;
}
// 빈 디렉토리 슬롯 찾기
static int dir_find_free(void) {
    for (int i = 0; i < MAX_FILES; i++)
        if (!g_dir[i].used) return i;
    return -1;
}

// 체인 해제
static void chain_free(int head) {
    while (head >= 0) {
        int nxt = g_fat[head];
        g_fat[head] = FAT_FREE;
        head = (nxt >= 0) ? nxt : -1;
    }
}

// 포맷 (초기화)
static void fs_format(void) {
    memset(g_dir, 0, sizeof(g_dir));
    for (int i = 0; i < NUM_BLKS; i++)
        g_fat[i] = FAT_FREE;
    memset(g_data, 0, sizeof(g_data));
}

// 저장
static int fs_save(void) {
    FILE *fp = fopen(IMG_NAME, "wb");
    if (!fp) return -1;
    unsigned int magic = 0x4B575546; // 'KWUF'
    unsigned int ver   = 1;
    fwrite(&magic, sizeof(magic), 1, fp);
    fwrite(&ver,   sizeof(ver),   1, fp);
    fwrite(g_dir,  sizeof(g_dir), 1, fp);
    fwrite(g_fat,  sizeof(g_fat), 1, fp);
    fwrite(g_data, sizeof(g_data),1, fp);
    fclose(fp);
    return 0;
}
// 복원
static int fs_load(void) {
    FILE *fp = fopen(IMG_NAME, "rb");
    if (!fp) return -1;
    unsigned int magic = 0, ver = 0;
    if (fread(&magic, sizeof(magic), 1, fp) != 1) { fclose(fp); return -1; }
    if (fread(&ver,   sizeof(ver),   1, fp) != 1) { fclose(fp); return -1; }
    if (magic != 0x4B575546) { fclose(fp); return -1; } // 매직 불일, 새 파일로 간주
    if (fread(g_dir,  sizeof(g_dir), 1, fp) != 1) { fclose(fp); return -1; }
    if (fread(g_fat,  sizeof(g_fat), 1, fp) != 1) { fclose(fp); return -1; }
    if (fread(g_data, sizeof(g_data),1, fp) != 1) { fclose(fp); return -1; }
    fclose(fp);
    return 0;
}

// 파일 생성 
static int cmd_create(const char *name) {
    if (!name || !*name) { fprintf(stderr, "create: invalid name\n"); return 1; }
    if (strlen(name) > NAME_MAXLEN) { fprintf(stderr, "create: name too long\n"); return 1; }
    if (dir_find_by_name(name) >= 0) { fprintf(stderr, "create: already exists\n"); return 1; }

    int slot = dir_find_free();
    if (slot < 0) { fprintf(stderr, "create: directory full\n"); return 1; }

    int b = blk_find_free();
    if (b < 0) { fprintf(stderr, "create: disk full\n"); return 1; }

    g_dir[slot].used = 1;
    strncpy(g_dir[slot].name, name, NAME_MAXLEN);
    g_dir[slot].name[NAME_MAXLEN] = '\0';
    g_dir[slot].size = 0;
    g_dir[slot].head = b;

    g_fat[b] = FAT_END;
    memset(g_data[b], 0, BLK_SZ);
    return 0;
}

// 파일 쓰기
static int cmd_write(const char *name, const char *content) {
    int idx = dir_find_by_name(name);
    if (idx < 0) { fprintf(stderr, "write: no such file\n"); return 1; }

    chain_free(g_dir[idx].head);

    if (!content) content = "";
    int n = (int)strlen(content);
    g_dir[idx].size = n;

    int need = (n + BLK_SZ - 1) / BLK_SZ;
    if (need == 0) {
        int b = blk_find_free(); if (b < 0) { fprintf(stderr, "write: disk full\n"); return 1; }
        g_dir[idx].head = b; g_fat[b] = FAT_END; memset(g_data[b], 0, BLK_SZ);
        return 0;
    }

    int first = -1, prev = -1;
    const unsigned char *src = (const unsigned char *)content;
    for (int i = 0; i < need; i++) {
        int b = blk_find_free();
        if (b < 0) { fprintf(stderr, "write: disk full (partial)\n"); chain_free(first); return 1; }
        if (first < 0) first = b;
        if (prev >= 0) g_fat[prev] = b;
        /* BLK_SZ만큼 복사(잔여는 0으로) */
        int chunk = (n > BLK_SZ) ? BLK_SZ : n;
        memset(g_data[b], 0, BLK_SZ);
        memcpy(g_data[b], src, chunk);
        src += chunk; n -= chunk;
        g_fat[b] = FAT_END; /* 일단 끝으로 마크, 다음 블록에서 이어짐 */
        prev = b;
    }
    g_dir[idx].head = first;
    return 0;
}
// 파일 읽기
static int cmd_read(const char *name) {
    int idx = dir_find_by_name(name);
    if (idx < 0) { fprintf(stderr, "read: no such file\n"); return 1; }

    int remain = g_dir[idx].size;
    int b = g_dir[idx].head;

    while (b >= 0 && remain > 0) {
        int out = (remain > BLK_SZ) ? BLK_SZ : remain;
        fwrite(g_data[b], 1, out, stdout);
        remain -= out;
        b = (g_fat[b] >= 0) ? g_fat[b] : -1;
    }
    if (g_dir[idx].size > 0) putchar('\n');
    return 0;
}
// 파일 삭제
static int cmd_delete(const char *name) {
    int idx = dir_find_by_name(name);
    if (idx < 0) { fprintf(stderr, "delete: no such file\n"); return 1; }
    chain_free(g_dir[idx].head);
    memset(&g_dir[idx], 0, sizeof(g_dir[idx]));
    return 0;
}
// 파일 목록 출력
static int cmd_list(void) {
    printf("%-3s %-*s %-6s %-5s\n", "No", 20, "Name", "Size", "Head");
    int k = 0;
    for (int i = 0; i < MAX_FILES; i++) {
        if (!g_dir[i].used) continue;
        printf("%-3d %-*s %-6d %-5d\n",
               k++, 20, g_dir[i].name, g_dir[i].size, g_dir[i].head);
    }
    return 0;
}
// 사용법 출력
static void usage(const char *p) {
    fprintf(stderr,
        "Usage:\n"
        "  %s create <name>\n"
        "  %s write  <name> \"text...\"\n"
        "  %s read   <name>\n"
        "  %s delete <name>\n"
        "  %s list\n"
        "  %s mkfs   (re-initialize volume)\n", p,p,p,p,p,p);
}

int main(int argc, char **argv)
{
    // 시작 시 복원
    if (fs_load() != 0)
        fs_format();

    int rc = 0;

    if (argc < 2) { usage(argv[0]); rc = 1; goto out; }

    if (strcmp(argv[1], "create") == 0) {
        if (argc != 3) { usage(argv[0]); rc = 1; }
        else rc = cmd_create(argv[2]);

    } else if (strcmp(argv[1], "write") == 0) {
        if (argc < 4) { usage(argv[0]); rc = 1; }
        else rc = cmd_write(argv[2], argv[3]);

    } else if (strcmp(argv[1], "read") == 0) {
        if (argc != 3) { usage(argv[0]); rc = 1; }
        else rc = cmd_read(argv[2]);

    } else if (strcmp(argv[1], "delete") == 0) {
        if (argc != 3) { usage(argv[0]); rc = 1; }
        else rc = cmd_delete(argv[2]);

    } else if (strcmp(argv[1], "list") == 0) {
        rc = cmd_list();

    } else if (strcmp(argv[1], "mkfs") == 0) {
        fs_format();
        printf("volume re-initialized.\n");
    } else {
        usage(argv[0]);
        rc = 1;
    }

out:
    // 종료 시 저장
    if (fs_save() != 0)
        fprintf(stderr, "warning: save failed (%s)\n", strerror(errno));
    return rc;
}
