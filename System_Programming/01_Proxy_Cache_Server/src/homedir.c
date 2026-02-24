#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>
#include <string.h>


char *getHomeDir(char *home) {
    struct passwd *usr_info = getpwuid(getuid());
    strcpy(home, usr_info->pw_dir);
    return home; //홈 경로를 반환합니다/
}
