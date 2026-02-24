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
#include <time.h>
#include <arpa/inet.h>
#define BUFFSIZE 1024               // 버퍼 크기 1024 바이트
#define PORTNO 40000                // 서버 포트 번호


int main() {


    bool result;
    int socket_fd, len;             
    struct sockaddr_in server_addr; 
    char haddr[] = "127.0.0.1";      // 서버 IP 주소 (localhost)
    char buf[BUFFSIZE];             // 송수신 데이터 버퍼
    

    
    // 소켓 생성
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Can't open stream socket\n");  // 소켓 생성 실패 시 메시지 출력
        return -1;                             // 프로그램 종료
    }

    // 버퍼와 서버 주소 구조체 초기화
    bzero(buf, sizeof(buf));                   // 버퍼를 0으로 초기화
    bzero((char *)&server_addr, sizeof(server_addr)); // 서버 주소 구조체를 0으로 초기화

    // 서버 주소 설정
    server_addr.sin_family = AF_INET;           // IPv4 주소체계 사용
    server_addr.sin_addr.s_addr = inet_addr(haddr); // 문자열 IP를 네트워크 주소로 변환
    server_addr.sin_port = htons(PORTNO);       // 포트 번호를 네트워크 바이트 순서로 변환

    // 서버에 연결 요청
    if (connect(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        printf("Can't connect.\n");             // 연결 실패 시 메시지 출력
        return -1;                              // 프로그램 종료
    }


    // 
    write(STDOUT_FILENO, "input url >", 12);
    while ((len = read(STDIN_FILENO, buf, BUFFSIZE)) > 0) {
        time_t server_start = time(NULL);
        buf[len - 1] = '\0'; // 개행 제거

        write(socket_fd, buf, strlen(buf)); // 서버에 전송
        // 서버로부터 "bye" 수신 시 종료
        if (strncmp(buf, "bye", 3) == 0) {
            time_t server_end = time(NULL);
            int runtime = (int)(server_end - server_start);
            write(socket_fd, &runtime, sizeof(runtime));  // runtime 전송
            break;
        }

        // 서버로부터 result 결과 수신
        if (read(socket_fd, &result, sizeof(result)) < 0) {
            perror("read result");
            continue;
        }
        // 결과에 따라 HIT 또는 MISS 출력
        if (result) {
            write(STDOUT_FILENO, "HIT\n", 4);
        } else {
            write(STDOUT_FILENO, "MISS\n", 5);
        }

        // 버퍼 초기화 후 다음 입력 대기
        write(STDOUT_FILENO, "input url >", 12);

    }

    // 통신 종료
    close(socket_fd);    // 소켓 연결 종료
    return 0;            // 프로그램 종료
}
