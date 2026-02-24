/////////////////////////////////////////////////////////////////////////
// File Name   : proxy_cache.c
// Date        : 2025/4/17
// OS          : Ubuntu 16.04 LTS 64-bit
// Author      : [junewoo kang]
// Student ID  : [2021202003]
// ------------------------------------------------------------------
// Title       :  System Programming Assignment 1-3
// Description :  url 데이터를 해쉬화 하여 경로와 피일로 저장합니다.
//                CMD 명령어를 통해서 connect와 quit 명령어를 받고
//                connect 명령어를 통해서 proxy_cache2를 실행합니다.
//                quit 명령어를 통해서 proxy_cache3를 종료합니다.
/////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>
#include <openssl/sha.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
/////////////////////////////////////////////////////////////////////////
// Function    : getHomeDir
// ---------------------------------------------------------------------
// Input       : 없음
// Output      : home - home 경로
// Description : home 경로에 대한 데이터를 반환합니다.
/////////////////////////////////////////////////////////////////////////
char *getHomeDir(char *home);

/////////////////////////////////////////////////////////////////////////
// Function    : sha1_hash
// ---------------------------------------------------------------------
// Input       : input_url  - URL 입력
// Output      : FILE *fp - 경로에 해쉬화된 파일 생성
// Description : URL을 입력받고 해쉬화된 경로로 저장합니다.
/////////////////////////////////////////////////////////////////////////

char* sha1_hash(char* input_url, char* hashed_url) {
    unsigned char hashed_160bits[20]; // SHA1 결과 20바이트 버퍼
    char hashed_hex[41]; // 40자리 16진수 문자열 + null 문자
    int i;

    SHA1((unsigned char*)input_url, strlen(input_url), hashed_160bits); // SHA1 해싱 수행
    for (i = 0; i < sizeof(hashed_160bits); i++) {
        sprintf(hashed_hex + i * 2, "%02x", hashed_160bits[i]);  // 해시 값을 16진수 문자열로 변환
    }

    strcpy(hashed_url, hashed_hex); // 결과 문자열 복사
    return hashed_url;
}

/////////////////////////////////////////////////////////////////////////
// Function    : proxy_cache2
// ---------------------------------------------------------------------
// Input       : logfile_path - 로그파일 경로
// Output      : 없음
// Description : URL을 입력받고 해쉬화된 경로와 이름으로 파일을 저장합니다.
/////////////////////////////////////////////////////////////////////////
void proxy_cache2(const char *logfile_path) {
    char input_url[256], hashed_url[41];
    char home[256], cache_root[512], logfile_root[512];
    char sub_dir[4], full_dir[1024], full_file[2048];
    int hit = 0, miss = 0;
    bool found = 0;
    time_t start = time(NULL);

    getHomeDir(home);
    umask(0000);

    while (1) {
        printf("[%d]input URL> ", getpid());
        scanf("%s", input_url); // 사용자로부터 URL 입력 받기

        if (strcmp(input_url, "bye") == 0) {
            // 수행 시간 계산산
            time_t end = time(NULL);
            int runtime = (int)(end - start);

            // logfile append 모드로 열기
            FILE *fp = fopen(logfile_path, "a");
            if (fp != NULL) { 
                fprintf(fp, "[Terminated] run time: %d sec. #request hit : %d, miss : %d\n",
                        runtime, hit, miss);
                fclose(fp);
            }
            break;
        }

        sha1_hash(input_url, hashed_url); // URL을 해시로 변환

        strcpy(cache_root, home);
        strcat(cache_root, "/cache");
        mkdir(cache_root, S_IRWXU | S_IRWXG | S_IRWXO); // cache 디렉토리 생성 777

        strncpy(sub_dir, hashed_url, 3); // 파일 제목은 해쉬 앞의 3자리 따오기기
        sub_dir[3] = '\0';

        strcpy(full_dir, cache_root); // 전체 디렉터리 경로 조합
        strcat(full_dir, "/");
        strcat(full_dir, sub_dir);
        mkdir(full_dir, S_IRWXU | S_IRWXG | S_IRWXO); // 하위 디렉토리 생성 777

        strcpy(full_file, full_dir);
        strcat(full_file, "/");
        strcat(full_file, hashed_url + 3); //파일 이름 설정


        DIR *subdir = opendir(cache_root); // "/home/user/cache" 열기
        struct dirent *entry;
        found = 0;

        if (subdir != NULL) {
            while ((entry = readdir(subdir)) != NULL) {

                // 1. sub_dir 디렉토리인지 확인
                if (strcmp(entry->d_name, sub_dir) == 0) {
                    // 2. 해당 디렉토리 안에 hashed_url+3 이름의 파일이 있는지 확인
                    char matched_dir[1024];
                    strcpy(matched_dir, cache_root);   
                    strcat(matched_dir, "/");          
                    strcat(matched_dir, sub_dir);      

                    DIR *subdir = opendir(matched_dir);
                    if (subdir != NULL) {
                        struct dirent *sub_entry;
                        while ((sub_entry = readdir(subdir)) != NULL) {
                            
                            // 파일명이 hashed_url + 3 과 같은지 확인
                            if (strcmp(sub_entry->d_name, hashed_url + 3) == 0) {
                                found = 1; 
                                break;
                            }
                        }
                        closedir(subdir);
                    }
                    break; // sub_dir 디렉토리는 하나뿐이니 찾았으면 종료
                }
            }
            closedir(subdir);
        }

        FILE *log_fp = fopen(logfile_path, "a"); // append 모드드
        if (found) { // 캐시가 존재하는 경우 hit 항목 기록록
            time_t now = time(NULL);
            struct tm *t = localtime(&now);
            char time_str[64];
            strftime(time_str, sizeof(time_str), "%Y/%m/%d, %H:%M:%S", t);

            fprintf(log_fp, "[Hit]%s/%s-[%s]\n", sub_dir, hashed_url + 3, time_str);
            fprintf(log_fp, "[Hit]%s\n", input_url);
            hit++;
        } else { // 캐시가 존재하지 않는 경우 miss 항목 기록록
            time_t now = time(NULL);
            struct tm *t = localtime(&now);
            char time_str[64];
            strftime(time_str, sizeof(time_str), "%Y/%m/%d, %H:%M:%S", t);

            fprintf(log_fp, "[Miss]%s-[%s]\n", input_url, time_str);
            miss++;

            FILE *cache_fp = fopen(full_file, "w");
            fprintf(cache_fp, "%s\n", input_url);
            fclose(cache_fp);
        }

        fclose(log_fp);
    }
}
///////////////////////////////////////////////////////////////////////////
// Function    : proxy_cache3
// ---------------------------------------------------------------------
// Input       : logfile_path - 로그파일 경로
// Output      : 없음
// Description : CMD내용 기반 fork를 통해서 proxy_cache2를 실행합니다.
/////////////////////////////////////////////////////////////////////////
void proxy_cache3(const char *logfile_path, time_t server_start) {
    char command[256];
    int sub_process_count = 0;

    while (1) {
        printf("[%d]input CMD> ", getpid());
        scanf("%s", command);

        if (strcmp(command, "connect") == 0) {
            pid_t pid = fork(); // 포크 실행 시작
            if (pid == 0) { // 자식의 경우
                proxy_cache2(logfile_path);
                exit(0);
            } else if (pid > 0) { // 부모의 경우
                sub_process_count++;
                wait(NULL);
            } else {
                perror("fork failed");
            }
        } else if (strcmp(command, "quit") == 0) {
            // 수행 시간 계산
            time_t server_end = time(NULL);
            int runtime = (int)(server_end - server_start);

            FILE *fp = fopen(logfile_path, "a"); //append 모드
            if (fp != NULL) {
                fprintf(fp, "**SERVER** [Terminated] run time: %d sec. #sub process: %d\n",
                        runtime, sub_process_count);
                fclose(fp);
            }
            break;
        }
    }
}


int main() {
    time_t server_start = time(NULL);
    char home[256], logfile_path[512];

    getHomeDir(home); // 홈 디렉터리 경로 가져오기

    strcpy(logfile_path, home);
    strcat(logfile_path, "/logfile");
    mkdir(logfile_path, S_IRWXU | S_IRWXG | S_IRWXO); // logfile 생성 777
    strcat(logfile_path, "/logfile.txt");

    proxy_cache3(logfile_path, server_start);

    return 0;
}
