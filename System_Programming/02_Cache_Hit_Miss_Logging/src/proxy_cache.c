/////////////////////////////////////////////////////////////////////////
// File Name   : proxy_cache.c
// Date        : 2025/4/10
// OS          : Ubuntu 16.04 LTS 64-bit
// Author      : [junewoo kang]
// Student ID  : [2021202003]
// ------------------------------------------------------------------
// Title       :  System Programming Assignment 1-2 
// Description :  URL데이터를 해쉬화 를 통해서 저장합니다. 만약 입력값이 bye라면 프로그램이 종료됩니다. 
//                Hit , Miss 카운트를 확인 및 기록합니다.
//                조건부 시간과 총 실행시간을 기록합니다.
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



int main() {
    char input_url[256];  // 사용자 입력 URL 저장용
    char hashed_url[41]; // SHA1 해시값 저장용 (40자리 + null)
    char home[256];      // 사용자 홈 디렉터리 경로
    getHomeDir(home);    // 홈 디렉터리 경로 가져오기
    char cache_root[512];// ~/cache 경로
    char logfile_root[512];// ~/logfile 경로로
    char sub_dir[4], full_dir[1024], full_file[1024], logfile_path[512], log_entry[1024]; // 디렉터리 및 파일 경로
    bool found = 0; // 파일 존재 여부
    umask(0000); // unmask 자동 권한 제한 방지지

    int hit = 0; // hit 카운트
    int miss = 0; // miss 카운트

    time_t t = time(NULL);
    
    time_t start = time(NULL);

    while (1) {
        printf("input url> ");
        scanf("%s", input_url); // 사용자로부터 URL 입력 받기
        if (strcmp(input_url, "bye") == 0) {
            time_t end = time(NULL);
            int runtime = (int)(end - start);

            // logfile 다시 append 모드로 열기
            FILE *log_fp_end = fopen(logfile_path, "a");
            if (log_fp_end != NULL) {
                fprintf(log_fp_end,
                    "[Terminated] run time: %d sec. #request hit : %d, miss : %d\n",
                    runtime, hit, miss);
                fclose(log_fp_end);
            } else {
                perror("logfile reopen failed");
            }

            break;
        }

        sha1_hash(input_url, hashed_url);  // URL을 해시로 변환
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

        strcpy(logfile_root, home);
        strcat(logfile_root, "/logfile");
        mkdir(logfile_root, S_IRWXU | S_IRWXG | S_IRWXO); // logfile 디렉토리 생성 777
        // logfile_root에 logfile.txt 파일 생성
        
        strcpy(logfile_path, logfile_root);
        strcat(logfile_path, "/logfile.txt");
        int logfile_fd = open(logfile_path, O_CREAT | O_WRONLY, 0777); // 파일 생성 777

        DIR *cache_dir = opendir(cache_root); // "/home/user/cache" 열기
        struct dirent *entry;
        found = 0;

        if (cache_dir != NULL) {
            while ((entry = readdir(cache_dir)) != NULL) {
                

                    // 1. sub_dir 디렉토리인지 확인
                    if (strcmp(entry->d_name, sub_dir) == 0) {
                        // 2. 해당 디렉토리 안에 hashed_url+3 이름의 파일이 있는지 확인
                        char matched_dir[1024];
                        snprintf(matched_dir, sizeof(matched_dir), "%s/%s", cache_root, sub_dir);

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
            closedir(cache_dir);
        }

        
        FILE *log_fp = fopen(logfile_path, "a");  // append 모드
        if (found) {
            fprintf(log_fp, "[Hit] %s/%s\n", sub_dir, hashed_url + 3);
            fprintf(log_fp, "[Hit] %s\n", input_url);
            hit++;
        } else {
            time_t now = time(NULL);
            struct tm *t = localtime(&now);
            char time_str[64];
            strftime(time_str, sizeof(time_str), "%Y/%m/%d, %H:%M:%S", t);

            fprintf(log_fp, "[Miss]%s -[%s]\n", input_url, time_str);
            miss++;

            // 캐시 파일 생성 + input_url 저장
            FILE *cache_fp = fopen(full_file, "w");
            fprintf(cache_fp, "%s\n", input_url);
            fclose(cache_fp);
            
        }

        fclose(log_fp);

            }

    return 0;
}