#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
void main(int argc, char *argv[])
{
    if(argc < 2){
        printf("error\n");
        return;
    }
    umask(0000);
    mkdir(argv[1], S_IRWXU | S_IRWXG | S_IRWXO);
}