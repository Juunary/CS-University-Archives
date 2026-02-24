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

#define BUFFSIZE  4096
#define PORTNO    39999

static int sub_process_count = 0;
static time_t server_start;

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
char *proxy_cache1_2(const char *url, bool *is_hit){
    char home[256], cache_root[512], hashed_url[41];
    char sub_dir[4], full_dir[1024], full_file[2048], logfile_path[512];
    bool found = false;

    //홈 디렉터리 및 캐시 루트 경로 설정
    getHomeDir(home);
    strcpy(cache_root, home);
    strcat(cache_root, "/cache");
    mkdir(cache_root, S_IRWXU | S_IRWXG | S_IRWXO);  // 777

    //로그 파일 경로 설정
    strcpy(logfile_path, home);
    strcat(logfile_path, "/logfile");
    mkdir(logfile_path, S_IRWXU | S_IRWXG | S_IRWXO); // 로그 디렉토리 생성
    strcat(logfile_path, "/logfile.txt");

    //URL 해시 생성
    sha1_hash(url, hashed_url);

    //하위 디렉터리 이름 설정 (해시 앞 3자리)
    strncpy(sub_dir, hashed_url, 3);
    sub_dir[3] = '\0';

    // 전체 디렉터리 경로 생성
    strcpy(full_dir, cache_root);
    strcat(full_dir, "/");
    strcat(full_dir, sub_dir);
    mkdir(full_dir, S_IRWXU | S_IRWXG | S_IRWXO);  // 777

    // 전체 캐시 파일 경로 생성 (해시 뒷부분)
    strcpy(full_file, full_dir);
    strcat(full_file, "/");
    strcat(full_file, hashed_url + 3);

    // 캐시 존재 여부 확인
    DIR *dp = opendir(full_dir);
    struct dirent *entry;
    if (dp != NULL) {
        while ((entry = readdir(dp)) != NULL) {
            if (strcmp(entry->d_name, hashed_url + 3) == 0) {
                found = true;
                break;
            }
        }
        closedir(dp);
    }

    // 로그용 시간 구하기
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char time_str[64];
    strftime(time_str, sizeof(time_str), "%Y/%m/%d, %H:%M:%S", t);

    // 로그 파일 기록
    FILE *log_fp = fopen(logfile_path, "a");
    if (log_fp != NULL) {
        if (found) {
            fprintf(log_fp, "[HIT]%s/%s-[%s]\n", sub_dir, hashed_url + 3, time_str);
            fprintf(log_fp, "[HIT]%s\n", url);
        } else {
            fprintf(log_fp, "[MISS]%s-[%s]\n", url, time_str);
        }
        fclose(log_fp);
    }

    *is_hit = found;
    // MISS/HIT 모두 캐시 파일 경로 반환 (HIT도 읽기 위해 필요)
    return strdup(full_file);
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
    _exit(1);  // 시그널-safe 종료
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
* Input       : url - 요청 URL (ex: http://www.columbia.edu/...)
* Output      : host - 호스트 이름 반환 (메모리 직접 할당, 사용 후 free 필요)
* Description : URL에서 호스트 이름을 파싱
**************************************************************************/
char* get_host_from_url(const char* url) {
    // "http://" or "https://" 이후 host 부분 추출
    const char *p = strstr(url, "://");
    if (p) p += 3;
    else p = url;
    const char *slash = strchr(p, '/');
    int len = slash ? (slash - p) : strlen(p);
    char *host = (char*)malloc(len + 1);
    strncpy(host, p, len);
    host[len] = '\0';
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
    const char *p = strstr(url, "://");
    if (p) p += 3;
    else p = url;
    const char *slash = strchr(p, '/');
    if (!slash) return strdup("/");
    else return strdup(slash);
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


int main(){
    int socket_fd;
    server_start = time(NULL);

    // SIGINT 핸들러 등록
    signal(SIGINT, sigint_handler);

    // 서버 소켓 생성
    if ((socket_fd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Server : Can't open stream socket\n");
        return 0;
    }

    // 주소 정보 초기화 및 바인딩 
    struct sockaddr_in server_addr;
    bzero((char*)&server_addr, sizeof(server_addr));
    server_addr.sin_family      = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port        = htons(PORTNO);

    if (bind(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        printf("Server : Can't bind local address\n");
        return 0;
    }

    // 리스닝 시작 
    listen(socket_fd, 10);

    // 좀비 프로세스 방지용 시그널 핸들러
    signal(SIGCHLD, (void *)handler);

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t clen = sizeof(client_addr);
        int client_fd = accept(socket_fd, (struct sockaddr*)&client_addr, &clen);
        if (client_fd < 0) continue; // 연결 시도

        if (fork() == 0) {
            // 자식 프로세스
            signal(SIGALRM, sigalcut_handler);
            close(socket_fd);  // 부모 소켓 닫기
            sub_process_count++;

            char buf[BUFFSIZE];                             // 클라이언트 요청 저장 버퍼
            char method[16], url[1024], proto[16];
            struct in_addr inet_client_address;
            inet_client_address.s_addr = client_addr.sin_addr.s_addr;

            printf("[%s : %d] client was connected\n",
                inet_ntoa(inet_client_address),
                ntohs(client_addr.sin_port));

            alarm(15); // 15초 동안 read 대기
            int n = read(client_fd, buf, BUFFSIZE); // 요청 안 들어오면 SIGALRM
            alarm(0); // 타이머 해제

            if (n <= 0) {
                close(client_fd);
                exit(1);
            }

            puts("======================================");
            printf("Request from [%s : %d]\n",              // 클라이언트 요청 로그 출력
                inet_ntoa(client_addr.sin_addr),
                ntohs(client_addr.sin_port));
            puts(buf);                                      // 요청 원문 출력
            puts("======================================");

            // url 추출
            sscanf(buf, "%15s %1023s %15s", method, url, proto);

            // favicon, 서브 리소스 등은 로그/캐시 제외 (아래 함수에서 필터링)
            if (!is_main_url(url)) {
                close(client_fd);
                exit(0);
            }

            bool is_hit;
            char *cache_path = proxy_cache1_2(url, &is_hit);

            if (is_hit) {
                // [HIT] 캐시 파일에서 읽어서 클라이언트에 전송
                FILE *fp = fopen(cache_path, "rb");
                if (fp) {
                    char tmp[BUFFSIZE];
                    int rn;
                    while ((rn = fread(tmp, 1, BUFFSIZE, fp)) > 0)
                        write(client_fd, tmp, rn);
                    fclose(fp);
                }
            } else {
                // [MISS] 웹 서버로 요청, 응답 받아 캐시에 저장, 클라이언트에 전송
                char *host = get_host_from_url(url);
                char *path = get_path_from_url(url);

                struct hostent *hp = gethostbyname(host);
                if (!hp) {
                    printf("Host not found: %s\n", host);
                    close(client_fd);
                    free(host);
                    free(path);
                    free(cache_path);
                    exit(1);
                }
                int web_fd = socket(PF_INET, SOCK_STREAM, 0);
                struct sockaddr_in web_addr;
                memset(&web_addr, 0, sizeof(web_addr));
                web_addr.sin_family = AF_INET;
                memcpy(&web_addr.sin_addr, hp->h_addr, hp->h_length);
                web_addr.sin_port = htons(80);

                if (connect(web_fd, (struct sockaddr*)&web_addr, sizeof(web_addr)) < 0) {
                    printf("Connect to web server failed\n");
                    close(client_fd);
                    free(host);
                    free(path);
                    free(cache_path);
                    exit(1);
                }

                // 클라이언트의 요청을 HTTP/1.0 GET 으로 재구성하여 웹서버에 보냄
                char request[2048];
                snprintf(request, sizeof(request),
                         "GET %s HTTP/1.0\r\nHost: %s\r\nConnection: close\r\n\r\n",
                         path, host);

                write(web_fd, request, strlen(request));

                // 응답 저장 및 전송
                FILE *cache_fp = fopen(cache_path, "wb");
                int rlen;
                char response[BUFFSIZE];
                while ((rlen = read(web_fd, response, BUFFSIZE)) > 0) {
                    // 캐시에 저장
                    if (cache_fp)
                        fwrite(response, 1, rlen, cache_fp);
                    // 클라이언트에 전송
                    write(client_fd, response, rlen);
                }
                if (cache_fp) fclose(cache_fp);
                close(web_fd);

                free(host);
                free(path);
            }

            free(cache_path);

            
            printf("[%s : %d] client was disconnected\n",
                inet_ntoa(client_addr.sin_addr),
                ntohs(client_addr.sin_port));
            puts("======================================");
            close(client_fd);
            exit(0);

        } else {
            // 부모 프로세스
            sub_process_count++;
            close(client_fd);  // 자식 프로세스가 client_fd 사용하므로 부모는 닫음
        }
    }

    close(socket_fd);
    return 0;
}
