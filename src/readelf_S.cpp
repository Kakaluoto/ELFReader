#include "header.h"

void readelf_S(const char* filename) {
    FILE* fp;
    Elf64_Ehdr elf_header;
    fp = fopen(filename, "r");
    if (fp == NULL) { exit(0); }
    fread(&elf_header, sizeof(Elf64_Ehdr), 1, fp);
    if (elf_header.e_ident[0] != 0x7f || elf_header.e_ident[1] != 'E') { exit(0); }
    //�������������洢�α���ÿһ��section_header,�ε���Ŀ:elf_header.e_shnum
    Elf64_Shdr* sec_headers = new Elf64_Shdr[elf_header.e_shnum];
    //��ָ���ƶ����α���ʼ��ַ������ʼ��ַelf_header.e_shoff�����������elf�ļ���ƫ������SEEK_SET���ļ���ʼ��ʼƫ��
    fseek(fp, elf_header.e_shoff, SEEK_SET);
    //��section_header��ÿһ��header��СΪsizeof(Elf64_Shdr)��һ����elf_header.e_shnum���α�ͷ
    fread(sec_headers, sizeof(Elf64_Shdr), elf_header.e_shnum, fp);
    printf("There are %d section headers, starting at offset 0x%lx\n\n", elf_header.e_shnum, elf_header.e_shoff);
    printf("Section Headers:\n");

    int str_tab_ind = elf_header.e_shstrndx;//��ȡ�ַ������ڶα��е�����elf_header.e_shstrndx��������ȡ����
    fseek(fp, sec_headers[str_tab_ind].sh_offset, SEEK_SET);//��ָ���ƶ����ַ�����
    char* string_table = new char[sec_headers[str_tab_ind].sh_size];//�����ַ����������洢�ַ���������ַ�
    fread(string_table, 1, sec_headers[str_tab_ind].sh_size, fp);//���ַ�����������ַ�ȫ��������

    //��ȡsection�ε�����,�������Ͷ�Ӧ����ֵ�������ַ����͵�������
    auto get_sh_type = [](int sh_type, string& sec_header_name) {
        switch (sh_type) {
            case SHT_NULL:
                sec_header_name = "NULL";
                break;
            case SHT_PROGBITS:
                sec_header_name = "PROGBITS";
                break;
            case SHT_SYMTAB:
                sec_header_name = "SYMTAB";
                break;
            case SHT_STRTAB:
                sec_header_name = "STRTAB";
                break;
            case SHT_RELA:
                sec_header_name = "RELA";
                break;
            case SHT_HASH:
                sec_header_name = "HASH";
                break;
            case SHT_DYNAMIC:
                sec_header_name = "DYNAMIC";
                break;
            case SHT_NOTE:
                sec_header_name = "NOTE";
                break;
            case SHT_NOBITS:
                sec_header_name = "NOBITS";
                break;
            case SHT_REL:
                sec_header_name = "REL";
                break;
            case SHT_SHLIB:
                sec_header_name = "SHLIB";
                break;
            case SHT_DYNSYM:
                sec_header_name = "DYNSYM";
                break;
            case SHT_INIT_ARRAY:
                sec_header_name = "INIT_ARRAY";
                break;
            case SHT_FINI_ARRAY:
                sec_header_name = "FINI_ARRAY";
                break;
            case SHT_PREINIT_ARRAY:
                sec_header_name = "PREINIT_ARRAY";
                break;
            case SHT_GROUP:
                sec_header_name = "GROUP";
                break;
            case SHT_SYMTAB_SHNDX:
                sec_header_name = "SYMTAB_SHNDX";
                break;
            case SHT_NUM:
                sec_header_name = "NUM";
                break;
            case SHT_GNU_HASH:
                sec_header_name = "GNU_HASH";
                break;
            case SHT_GNU_versym:
                sec_header_name = "VERSYM";
                break;
            case SHT_GNU_verneed:
                sec_header_name = "VERNEED";
                break;
            default:
                sec_header_name = "UnknownType";
        }
    };

    //��ȡsection�εı�־flag,�������Ͷ�Ӧ����ֵ�������ַ����͵�flag
    //��Ϊ����ͬʱ������flag�����Ը��ݶ�Ӧλ�Ƿ�Ϊ1���ж��Ƿ������Ӧ��flag����ͽ�flag�ַ���ƴ��
    auto get_sh_flags = [](unsigned int sh_flags, string& sec_header_name) {
        if ((sh_flags & SHF_WRITE) >> 0)
            sec_header_name += "W";
        if ((sh_flags & SHF_ALLOC) >> 1)
            sec_header_name += "A";
        if ((sh_flags & SHF_EXECINSTR) >> 2)
            sec_header_name += "X";
        if ((sh_flags & SHF_MERGE) >> 4)
            sec_header_name += "M";
        if ((sh_flags & SHF_STRINGS) >> 5)
            sec_header_name += "S";
        if ((sh_flags & SHF_INFO_LINK) >> 6)
            sec_header_name += "I";
        if ((sh_flags & SHF_LINK_ORDER) >> 7)
            sec_header_name += "L";
        if ((sh_flags & SHF_OS_NONCONFORMING) >> 8)
            sec_header_name += "O";
        if ((sh_flags & SHF_GROUP) >> 9)
            sec_header_name += "G";
        if ((sh_flags & SHF_TLS) >> 10)
            sec_header_name += "T";
        if ((sh_flags & SHF_COMPRESSED) >> 11)
            sec_header_name += "C";
        //����flag��Ϊ��Ӧ��λ�������flag��Ӧ��λ���ص������Կ��Ե�������
        switch (sh_flags) {
            case SHF_MASKOS:
                sec_header_name = "o";
                break;
            case SHF_MASKPROC:
                sec_header_name = "p";
                break;
            case SHF_EXCLUDE:
                sec_header_name = "E";
                break;
        }
    };

    printf("  [Nr]\tName\t\t\tType\t\tAddr\t\tOffset\t\tSize\t\t"
           "EntSize\t\tFlags\tLink\tInfo\tAlign\n");
    //����section_headers�α����ÿ��section,�����Ӧ����Ϣ
    for (int i = 0; i < elf_header.e_shnum; i++) {
        printf("  [%2d]\t", i);
        printf("%-24s", &string_table[sec_headers[i].sh_name]);
        string sh_type;
        get_sh_type(sec_headers[i].sh_type, sh_type);
        printf("%-16s", sh_type.data());
        printf("0x%08lx\t", sec_headers[i].sh_addr);
        printf("0x%08lx\t", sec_headers[i].sh_offset);
        printf("0x%08lx\t", sec_headers[i].sh_size);
        printf("0x%08lx\t", sec_headers[i].sh_entsize);
        string sh_flags;
        get_sh_flags(sec_headers[i].sh_flags, sh_flags);
        printf("%-8s", sh_flags.data());
        printf("%-8d", sec_headers[i].sh_link);
        printf("%-8d", sec_headers[i].sh_info);
        printf("%-8ld", sec_headers[i].sh_addralign);
        printf("\n");
    }
    printf("Key to Flags:\n"
           "\tW (write), A (alloc), X (execute), M (merge), S (strings), I (info),\n"
           "\tL (link order), O (extra OS processing required), G (group), T (TLS),\n"
           "\tC (compressed), x (unknown), o (OS specific), E (exclude),\n"
           "\tl (large), p (processor specific)\n");

    //�ͷŶ��ڴ�
    delete[] string_table;
    delete[] sec_headers;
    fclose(fp);
}
