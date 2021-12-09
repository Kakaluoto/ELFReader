#include "header.h"


int main(int argc, char* argv[]) {
    if (argc < 2) { exit(0); }
    //argv[0]�ǵ�ǰ��ִ���ļ�·��
    //arv[1]�ǲ����������Կո���Ϊ�ָ���
    //argv[2]�Ǳ������Ŀ�ִ���ļ���
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

