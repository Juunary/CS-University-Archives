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

#define BUFFSIZE  4096
#define PORTNO    39999

/////////////////////////////////////////////////////////////////////////
// Function    : getHomeDir
// ---------------------------------------------------------------------
// Input       : 없음
// Output      : home - home 경로
// Description : home 경로에 대한 데이터를 반환합니다.
/////////////////////////////////////////////////////////////////////////
char *getHomeDir(char *home) {
    struct passwd *pw = getpwuid(getuid());
    strcpy(home, pw->pw_dir);
    return home;
}

//////////////////////////////////////////////////////////////////////////
// Function    : sha1_hash
// ---------------------------------------------------------------------
// Input       : input_url  - URL 입력
// Output      : hashed_url - 해시된 URL
// Description : URL을 입력받고 해시화된 경로로 저장합니다.
/////////////////////////////////////////////////////////////////////////
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

/////////////////////////////////////////////////////////////////////////
// Function    : proxy_cache1_2
// ---------------------------------------------------------------------
// Input       : url 
// Output      : bool - 캐시 히트 여부
// Description : 캐시 디렉토리 생성 및 해시화된 URL로 파일 저장
/////////////////////////////////////////////////////////////////////////
bool proxy_cache1_2(const char *url) {
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
            fprintf(log_fp, "[Hit]%s/%s-[%s]\n", sub_dir, hashed_url + 3, time_str);
            fprintf(log_fp, "[Hit]%s\n", url);
        } else {
            fprintf(log_fp, "[Miss]%s-[%s]\n", url, time_str);
        }
        fclose(log_fp);
    }

    // 캐시 MISS인 경우 빈 파일 생성
    if (!found) {
        FILE *fp = fopen(full_file, "w");
        if (fp != NULL) fclose(fp);
    }

    return found;

}

///////////////////////////////////////////////////////////////////////////
// Function    : handler
// ---------------------------------------------------------------------
// Input       : 없음
// Output      : 없음
// Description : 자식 프로세스 종료 시그널 처리
///////////////////////////////////////////////////////////////////////////
static void handler(){
    pid_t pid;
    int status;
    // 자식 프로세스 종료 시그널 처리
    while((pid = waitpid(-1, &status, WNOHANG)) > 0);
}


int main(){

    // 서버 소켓 생성성
    int socket_fd;
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

        // 클라이언트 주소 구조체 초기화
        struct sockaddr_in client_addr;
        socklen_t clen = sizeof(client_addr);
        int client_fd = accept(socket_fd, (struct sockaddr*)&client_addr, &clen);
        if (client_fd < 0) continue; // 연결 시도


        if (fork() == 0) {
            // 자식 프로세스
            close(socket_fd);  // 부모 소켓 닫기
            // 클라이언트 주소 구조체 초기화
            char buf[BUFFSIZE];                             // 클라이언트 요청 저장 버퍼
            char response_header[BUFFSIZE]  = {0,};         // 응답 헤더 버퍼
            char response_message[BUFFSIZE] = {0,};         // 응답 본문 버퍼

            struct in_addr inet_client_address;
            inet_client_address.s_addr = client_addr.sin_addr.s_addr;

            memset(response_header,  0, sizeof(response_header));
            memset(response_message, 0, sizeof(response_message));

            printf("[%s : %d] client was connected\n",
                inet_ntoa(inet_client_address),
                ntohs(client_addr.sin_port));

        
            read(client_fd, buf, BUFFSIZE);                 // 클라이언트 요청 메시지 읽기

            
            puts("======================================");
            printf("Request from [%s : %d]\n",              // 클라이언트 요청 로그 출력
                inet_ntoa(client_addr.sin_addr),
                ntohs(client_addr.sin_port));
            puts(buf);                                      // 요청 원문 출력
            puts("======================================");

            // url 추출
            char method[16], url[1024], proto[16];
            sscanf(buf, "%15s %1023s %15s", method, url, proto);

            // HIT/MISS 판별
            bool found = proxy_cache1_2(url);

            // Response Body 생성
            if (found) {
                sprintf(response_message,
                    "<h1>HIT</h1><br>"
                    "%s:%d<br>"
                    "%s<br>"
                    "kw2021202003<br>",
                    inet_ntoa(inet_client_address),
                    ntohs(client_addr.sin_port),
                    url);
            } else {
                sprintf(response_message,
                    "<h1>MISS</h1><br>"
                    "%s:%d<br>" 
                    "%s<br>"
                    "kw2021202003<br>",
                    inet_ntoa(inet_client_address),
                    ntohs(client_addr.sin_port),
                    url);
            }
            // 응답 헤더 문자열 생성
            sprintf(response_header,                        
                "HTTP/1.0 200 OK\r\n"
                "Server: 2018 simple web server\r\n"
                "Content-length: %lu\r\n"
                "Content-type: text/html\r\n\r\n",
                strlen(response_message));

            // 클라이언트로 헤더 + 본문 전송 
            write(client_fd, response_header,  strlen(response_header));   // 헤더 전송
            write(client_fd, response_message, strlen(response_message)); // 본문 전송

            // 소켓 닫기
            printf("[%s : %d] client was disconnected\n",
                inet_ntoa(client_addr.sin_addr),
                ntohs(client_addr.sin_port));
            close(client_fd);
            exit(0);
        } else {
            // 부모 프로세스스
            close(client_fd);  // 자식 프로세스가 client_fd 사용하므로 부모는 닫음
        }

    }

    close(socket_fd);
    return 0;
}
