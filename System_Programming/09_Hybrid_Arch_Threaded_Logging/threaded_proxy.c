#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>
#include <time.h>
#include <openssl/sha.h>
#include <stdbool.h>
#include <netdb.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>
#include <pthread.h>

#define BUFFSIZE 4096
#define PORTNO   39999

// 전역 변수
static int sub_process_count = 0;
static time_t server_start;
static int semid; // 세마포어 ID

// System V semun 정의
union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

// 로그 파라미터 구조체 정의
typedef struct log_param {
    char logpath[512];
    bool found;
    char sub[4];
    char *hash3;
    char ts[64];
    char url[1024];
} log_param;

// stdout 논버퍼링
static void init_output(void) {
    setbuf(stdout, NULL);
}
/**************************************************************************
* Function    : write_log_thread
* ------------------------------------------------------------------------
* Input       : arg - 로그 파라미터 구조체 포인터
* Output      : 없음
* Description : 로그 기록 thread 함수
**************************************************************************/
void* write_log_thread(void* arg) {
    log_param* param = (log_param*)arg;
    FILE *fp = fopen(param->logpath, "a");
    if (fp) {
        fprintf(fp, param->found ? "[HIT]%s/%s-[%s]\n" : "[MISS]%s-[%s]\n",
                param->sub, param->hash3, param->ts);
        fprintf(fp, param->found ? "[HIT]%s\n" : "", param->url);
        fclose(fp);
    }
    printf("*TID# %ld is exited.\n", pthread_self());
    return NULL;
}

/**************************************************************************
* Function    : getHomeDir
* ------------------------------------------------------------------------
* Input       : 없음
* Output      : home - home 경로
* Description : home 경로에 대한 데이터를 반환합니다.
**************************************************************************/
char *getHomeDir(char *home) {
    struct passwd *pw = getpwuid(getuid());
    strcpy(home, pw->pw_dir);
    return home;
}

/**************************************************************************
* Function    : sha1_hash
* ------------------------------------------------------------------------
* Input       : input_url  - URL 입력
* Output      : hashed_url - 해시된 URL
* Description : URL을 입력받고 해시화된 경로로 저장합니다.
**************************************************************************/
char* sha1_hash(const char* input_url, char* hashed_url) {
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

/**************************************************************************
* Function    : proxy_cache1_2
* ------------------------------------------------------------------------
* Input       : url 
* Output      : char* - 캐시 파일 경로
* Description : 캐시 디렉토리 생성 및 해시화된 URL로 파일 저장
*               MISS/HIT 모두 cache 파일 경로 반환
**************************************************************************/
char* proxy_cache1_2(const char *url, bool *is_hit) {
    char home[256], cache_root[512], hash[41];
    char sub[4], dir[1024], file[2048], logpath[512];
    bool found = false;
    // 홈 디렉터리 및 캐시 루트 경로 설정
    getHomeDir(home);
    snprintf(cache_root, sizeof(cache_root), "%s/cache", home);
    mkdir(cache_root, 0777);
    // 로그 파일 경로 설정
    snprintf(logpath, sizeof(logpath), "%s/logfile/logfile.txt", home);
    mkdir(strcat(strcpy(dir, home), "/logfile"), 0777);

    sha1_hash(url, hash);
    strncpy(sub, hash, 3); sub[3] = '\0';

    snprintf(dir, sizeof(dir), "%s/%s", cache_root, sub);
    mkdir(dir, 0777);

    snprintf(file, sizeof(file), "%s/%s", dir, hash+3);
    // 캐시 파일이 존재하는지 확인
    DIR *dp = opendir(dir);
    struct dirent *e;
    if (dp) {
        while ((e = readdir(dp))) if (!strcmp(e->d_name, hash+3)) { found = true; break; }
        closedir(dp);
    }

    // 세마포어 초기화
    struct sembuf sb = {0, -1, SEM_UNDO};
    printf("*PID# %d is waiting for the semaphore.\n", getpid());
    semop(semid, &sb, 1);

    // critical zone
    printf("*PID# %d is in the critical zone.\n", getpid());
    sleep(1);

    // 로그 기록 파라미터 준비
    time_t now = time(NULL);
    struct tm *tm = localtime(&now);
    char ts[64];
    strftime(ts, sizeof(ts), "%Y/%m/%d, %H:%M:%S", tm);

    pthread_t tid;
    log_param param;
    strncpy(param.logpath, logpath, sizeof(param.logpath));
    param.found = found;
    strncpy(param.sub, sub, sizeof(param.sub));
    param.hash3 = (char*)(hash+3);
    strncpy(param.ts, ts, sizeof(param.ts));
    strncpy(param.url, url, sizeof(param.url));

    // thread 생성
    printf("*PID# %d create the *TID# ", getpid());
    fflush(stdout);
    pthread_create(&tid, NULL, write_log_thread, (void*)&param);
    printf("%ld.\n", tid);

    // thread 종료 대기
    pthread_join(tid, NULL);

    printf("*PID# %d exited the critical zone.\n", getpid());

    // 세마포어 해제
    sb.sem_op = 1;
    semop(semid, &sb, 1);
    // critical zone 종료
    *is_hit = found;
    return strdup(file);
}

/**************************************************************************
* Function    : handler
* ------------------------------------------------------------------------
* Input       : 없음
* Output      : 없음
* Description : 자식 프로세스 종료 시그널 처리
**************************************************************************/
static void handler(){
    pid_t pid;
    int status;
    // 자식 프로세스 종료 시그널 처리
    while((pid = waitpid(-1, &status, WNOHANG)) > 0);
}
/**************************************************************************
* Function    : sigalcut_handler
* ------------------------------------------------------------------------
* Input       : signo - 시그널 번호
* Output      : 없음
* Description : SIGALRM 시그널 핸들러
**************************************************************************/
void sigalcut_handler(int signo) {
    printf("응답 없음\n");
    _exit(1);  // 시그널 safe 종료
}

/**************************************************************************
* Function    : sigint_handler
* ------------------------------------------------------------------------
* Input       : sig - 시그널 번호
* Output      : 없음
* Description : SIGINT 시그널 핸들러 (Ctrl+C 종료 로그)
**************************************************************************/
void sigint_handler(int sig) {
    time_t now = time(NULL);
    int elapsed = (int)(now - server_start);
    
    char home[256], logfile_path[512];
    getHomeDir(home);
    strcpy(logfile_path, home);
    strcat(logfile_path, "/logfile/logfile.txt");

    FILE *fp = fopen(logfile_path, "a");
    if (fp) {
        fprintf(fp, "**SERVER** [Terminated] run time: %d sec. #sub process: %d\n", elapsed, sub_process_count);
        fclose(fp);
    }
    exit(0);
}


/**************************************************************************
* Function    : get_host_from_url
* ------------------------------------------------------------------------
* Input       : url - 요청 URL 
* Output      : host - 호스트 이름 반환 (메모리 직접 할당, 사용 후 free 필요)
* Description : URL에서 호스트 이름을 파싱
**************************************************************************/
char* get_host_from_url(const char* url) {
    // URL에서 "://" 부분을 찾아 호스트명 시작 위치 파악
    const char *start = strstr(url, "://");
    if (start != NULL) {
        start += 3;
    } else {
        start = url;
    }

    // '/'를 찾아 호스트 부분 끝 위치 파악
    const char *slash = strchr(start, '/');
    int len;
    if (slash != NULL) {
        len = slash - start; // 호스트명만큼의 길이
    } else {
        len = strlen(start); // 슬래시가 없으면 전체가 호스트
    }

    // 메모리 할당 및 복사
    char *host = (char*)malloc(len + 1);
    if (host == NULL) {
        // 메모리 할당 실패 시 NULL 반환
        return NULL;
    }
    strncpy(host, start, len);
    host[len] = '\0'; // 널 종료

    return host;
}


/**************************************************************************
* Function    : get_path_from_url
* ------------------------------------------------------------------------
* Input       : url - 요청 URL
* Output      : path - 경로 반환 
* Description : URL에서 path 부분 파싱, 없으면 "/" 반환
**************************************************************************/
char* get_path_from_url(const char* url) {
    // "://"가 있는 경우 path 시작 부분을 찾는다
    const char *scheme_pos = strstr(url, "://");
    const char *host_start;
    if (scheme_pos != NULL) {
        // "://" 다음이 host 시작 위치
        host_start = scheme_pos + 3;
    } else {
        // "://"가 없으면 전체가 host + path로 가정
        host_start = url;
    }

    // '/'로 path 시작 위치를 찾는다
    const char *path_start = strchr(host_start, '/');

    if (path_start == NULL) {
        // '/'가 없으면 path는 "/"로 간주
        return strdup("/");
    } else {
        // '/'부터 끝까지가 path
        return strdup(path_start);
    }
}

/**************************************************************************
* Function    : is_main_url
* ------------------------------------------------------------------------
* Input       : url - 요청 URL 
* Output      : bool - 메인 URL 여부
* Description : URL에서 메인 URL인지 확인
**************************************************************************/
bool is_main_url(const char* url) {
    // favicon, 이미지, CSS, JS 등은 모두 제외
    const char *ext[] = {
        ".ico", ".gif", ".png", ".jpg", ".jpeg", ".bmp", ".svg", ".css", ".js", ".webp",
        ".woff", ".woff2", ".ttf", ".otf", ".eot", ".mp4", ".mp3", ".avi", ".mov"
    };
    for (int i = 0; i < sizeof(ext)/sizeof(ext[0]); i++) {
        if (strstr(url, ext[i])) 
        return false;
    }
    // path가 '/' 이거나 .html, .htm로 끝나면 메인
    char *path = get_path_from_url(url);
    bool res = false;
    if (strcmp(path, "/") == 0) res = true;
    if (strstr(path, ".html") || strstr(path, ".htm")) res = true;
    free(path);
    return res;
}



int main() {
    init_output();
    server_start = time(NULL);

    // 세마포어 초기화
    if ((semid = semget(PORTNO, 1, IPC_CREAT|IPC_EXCL|0666)) >= 0) {
        union semun arg = {.val = 1};
        semctl(semid, 0, SETVAL, arg);
    } else if (errno == EEXIST) {
        semid = semget(PORTNO, 1, 0666);
    } else {
        perror("semget");
        return 1;
    }
    // 시그널 핸들러 설정
    signal(SIGINT, sigint_handler);
    signal(SIGCHLD, handler);
    // 소켓 생성 및 설정
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = {AF_INET, htons(PORTNO), INADDR_ANY};
    bind(sock, (struct sockaddr*)&addr, sizeof(addr));
    listen(sock, 10);

    while (1) {
        // 클라이언트 연결 수락
        struct sockaddr_in caddr;
        socklen_t clen = sizeof(caddr);
        int cfd = accept(sock, (struct sockaddr*)&caddr, &clen);
        if (cfd < 0) continue;
        // 최대 자식 프로세스 포크크
        if (!fork()) {
            signal(SIGALRM, sigalcut_handler);
            close(sock);
            sub_process_count++;
            // 클라이언트 요청 처리
            char buf[BUFFSIZE], method[16], url[1024], proto[16];
            alarm(15);
            int n = read(cfd, buf, sizeof(buf));
            alarm(0);
            if (n <= 0) exit(1);

            sscanf(buf, "%15s %1023s %15s", method, url, proto);
            if (!is_main_url(url)) exit(0);
            // 캐시 확인 및 처리
            bool hit;
            char *cp = proxy_cache1_2(url, &hit);
            // hit 경우 : 캐시 파일에서 읽어서 클라이언트에 전송
            if (hit) {
                FILE *f = fopen(cp, "rb");
                while (f && !feof(f)) {
                    char t[BUFFSIZE];
                    int r = fread(t,1,BUFFSIZE,f);
                    write(cfd,t,r);
                }
                if (f) fclose(f);
                // miss 경우 : 웹 서버로 요청, 응답 받아 캐시에 저장, 클라이언트에 전송
            } else {
                char *host = get_host_from_url(url);
                char *path = get_path_from_url(url);
                struct hostent *hp = gethostbyname(host);
                if (!hp) exit(1);
                int wfd = socket(AF_INET, SOCK_STREAM, 0);
                struct sockaddr_in waddr = {AF_INET, htons(80)};
                memcpy(&waddr.sin_addr, hp->h_addr, hp->h_length);
                connect(wfd, (struct sockaddr*)&waddr, sizeof(waddr));

                char req[2048];
                snprintf(req,sizeof(req),"GET %s HTTP/1.0\r\nHost: %s\r\nConnection: close\r\n\r\n", path, host);
                write(wfd, req, strlen(req));

                FILE *cf = fopen(cp, "wb");
                char resp[BUFFSIZE];
                int r;
                while ((r = read(wfd, resp, BUFFSIZE))>0) {
                    if (cf) fwrite(resp,1,r,cf);
                    write(cfd, resp, r);
                }
                if (cf) fclose(cf);
                close(wfd);
                free(host);
                free(path);
            }
            free(cp);
            close(cfd);
            exit(0);
        }
        close(cfd);
    }
    close(sock);
    return 0;
}
