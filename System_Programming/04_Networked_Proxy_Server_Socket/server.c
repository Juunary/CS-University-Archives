#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <wait.h>
#include <stdio.h>                  // printf, perror 등 표준 입출력 함수
#include <string.h>                 // 문자열 관련 함수(bzero, strlen 등)
#include <unistd.h>                 // read, write, close 함수
#include <sys/types.h>              // 소켓 통신용 데이터 타입
#include <sys/socket.h>             // socket, connect 함수
#include <sys/stat.h>              // mkdir 함수
#include <dirent.h>                 // 디렉토리 관련 함수
#include <time.h>                  // 시간 관련 함수
#include <netinet/in.h>             // sockaddr_in 구조체 정의
#include <openssl/sha.h>
#include <stdbool.h>   
#include <pwd.h>
#include <fcntl.h>


#define BUFFSIZE 1024
#define PORTNO 40000

int hit = 0, miss = 0;
/////////////////////////////////////////////////////////////////////////
// Function    : getHomeDir
// ---------------------------------------------------------------------
// Input       : 없음
// Output      : home - home 경로
// Description : home 경로에 대한 데이터를 반환합니다.
/////////////////////////////////////////////////////////////////////////
char *getHomeDir(char *home) {
    struct passwd *usr_info = getpwuid(getuid());
    strcpy(home, usr_info->pw_dir);
    return home; //홈 경로를 반환합니다/
}

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
// Function    : proxy_cache1_2
// ---------------------------------------------------------------------
// Input       : logfile_path - 로그 파일 경로
//               input_url    - 입력 URL
// Output      : bool - 캐시 히트 여부
// Description : 캐시 디렉토리 생성 및 해쉬화된 URL로 파일 저장
/////////////////////////////////////////////////////////////////////////
bool proxy_cache1_2(const char *logfile_path, char *input_url){
    char hashed_url[41];
    char home[256], cache_root[512], logfile_root[512];
    char sub_dir[4], full_dir[1024], full_file[2048];
    bool found = 0;
    
    getHomeDir(home);
    umask(0000);

    // 캐시 디렉토리 설정
    strcpy(cache_root, home);
    strcat(cache_root, "/cache");
    mkdir(cache_root, S_IRWXU | S_IRWXG | S_IRWXO); // 0777

    sha1_hash(input_url, hashed_url); // URL을 해시로 변환

    strncpy(sub_dir, hashed_url, 3);
    sub_dir[3] = '\0';
    // 서브 디렉토리 생성
    strcpy(full_dir, cache_root);
    strcat(full_dir, "/");
    strcat(full_dir, sub_dir);
    mkdir(full_dir, S_IRWXU | S_IRWXG | S_IRWXO);
    // 해시된 URL을 파일 이름으로 사용
    strcpy(full_file, full_dir);
    strcat(full_file, "/");
    strcat(full_file, hashed_url + 3);
    // 해시된 URL을 파일 이름으로 사용
    DIR *dir = opendir(full_dir);
    if (dir) {
        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL) {
            if (strcmp(entry->d_name, hashed_url + 3) == 0) {
                found = 1;
                break;
            }
        }
        closedir(dir);
    }
    
    FILE *log_fp = fopen(logfile_path, "a");
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char time_str[64];
    strftime(time_str, sizeof(time_str), "%Y/%m/%d, %H:%M:%S", t);
    input_url[strcspn(input_url, "\n")] = '\0';
    // 로그 파일에 기록
    if (found) {// 캐시 히트
        fprintf(log_fp, "[Hit] ServerPID : %d | %s/%s-[%s]\n", getpid(), sub_dir, hashed_url + 3, time_str);
        fprintf(log_fp, "[Hit]%s\n", input_url);
        fflush(log_fp); 
        return true;
        
    } else {// 캐시 미스
        fprintf(log_fp, "[Miss] ServerPID : %d | %s-[%s]\n", getpid(), input_url, time_str);
        FILE *cache_fp = fopen(full_file, "w");
        if (cache_fp) {
            fprintf(cache_fp, "%s\n", input_url);
            fclose(cache_fp);
            fflush(log_fp); 
        }
        return false;
        
    }

    fclose(log_fp);
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
    while((pid = waitpid(-1, &status, WNOHANG)) > 0);
}

int main(){

    char home[256], logfile_path[512];

    getHomeDir(home); // 홈 디렉터리 경로 가져오기

    strcpy(logfile_path, home);
    strcat(logfile_path, "/logfile");
    mkdir(logfile_path, S_IRWXU | S_IRWXG | S_IRWXO); // logfile 생성 777
    strcat(logfile_path, "/logfile.txt");

    
    struct sockaddr_in server_addr, client_addr;
    int socket_fd, client_fd;
    int len, len_out;
    int state;
    char buf[BUFFSIZE];
    pid_t pid;
    int hit = 0, miss = 0;
    // 소켓 생성 및 초기화
    if((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        printf("Server : Can't open stream socket\n");
        return 0;
    }

    bzero((char *)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORTNO);
    // 소켓 주소 구조체 초기화
    if(bind(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
        printf("Server : Can't bind local address\n");
        close(socket_fd);
        return 0;
    }
    
    listen(socket_fd, 5);
    signal(SIGCHLD, (void *)handler);

    while (1) {
        // 클라이언트 주소 구조체 초기화
        bzero((char *)&client_addr, sizeof(client_addr));
        len = sizeof(client_addr);
        // 클라이언트 연결 요청 수락
        client_fd = accept(socket_fd, (struct sockaddr *)&client_addr, &len);
        // 연결 실패 시 에러 출력 후 서버 소켓 종료
        if (client_fd < 0) {
            printf("Server : Accept failed %d\n", getpid());
            close(socket_fd);
            return 0;
        }
        // 연결된 클라이언트의 IP 및 포트 출력
        printf("[%d : %d] client was connected\n", client_addr.sin_addr.s_addr, client_addr.sin_port);
        
        pid = fork();
    
        if (pid == -1) {
            close(client_fd);
            close(socket_fd);
            continue;
        }
    
        if (pid == 0) {// 자식 프로세스: 각 클라이언트 전담 처리
            hit = 0; miss = 0;
            while ((len_out = read(client_fd, buf, BUFFSIZE)) > 0) {
                buf[len_out] = '\0';  // 널 종료문자 추가
        
                if (!strncmp(buf, "bye", 3)) {
                    int runtime;
                    read(client_fd, &runtime, sizeof(runtime)); // 클라이언트로부터 runtime 수신
                    // 로그 파일에 종료 정보 기록
                    FILE *fp = fopen(logfile_path, "a");
                    if (fp != NULL) {
                        fprintf(fp, "[Terminated] ServerPID : %d | run time : %d sec. #request hit : %d, miss : %d\n",
                                getpid(), runtime, hit, miss);
                        fclose(fp);
                    }
                    break;
                } else {
                    // URL 캐시 체크 및 처리
                    bool result = proxy_cache1_2(logfile_path, buf);
                    // 클라이언트에게 HIT 또는 MISS 여부 전송
                    write(client_fd, &result, sizeof(result));
                    if (result) hit++;
                    else miss++;
                    
                }
        
                
            }
            // 클라이언트 접속 종료 시 로그 출력 및 소켓 닫기
            printf("[%d : %d] client was disconnected\n", client_addr.sin_addr.s_addr, client_addr.sin_port);
            close(client_fd);
            exit(0);
        } else {
            // 부모 프로세스: 클라이언트 종료
            close(client_fd);
        }
    }
    
    close(socket_fd);
    return 0;
}