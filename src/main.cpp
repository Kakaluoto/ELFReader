#include "header.h"


int main(int argc, char* argv[]) {
    if (argc < 2) { exit(0); }
    //argv[0]是当前可执行文件路径
    //arv[1]是参数，参数以空格作为分隔符
    //argv[2]是被解析的可执行文件名
    if (strcmp(argv[1], "-h") == 0)
        readelf_h(argv[2]);
    else if (strcmp(argv[1], "-S") == 0)
        readelf_S(argv[2]);
    else if (strcmp(argv[1], "-s") == 0)
        readelf_s(argv[2]);
    else
        printf("invalid argument!\n");
    return 0;
}

