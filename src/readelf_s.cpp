#include "header.h"

void readelf_s(const char* filename) {
    FILE* fp;
    Elf64_Ehdr elf_header;
    fp = fopen(filename, "r");
    if (fp == NULL) { exit(0); }
    fread(&elf_header, sizeof(Elf64_Ehdr), 1, fp);
    if (elf_header.e_ident[0] != 0x7f || elf_header.e_ident[1] != 'E') { exit(0); }
    Elf64_Shdr* sec_headers = new Elf64_Shdr[elf_header.e_shnum];//���ÿ��section_header������
    fseek(fp, elf_header.e_shoff, SEEK_SET);//�ƶ�ָ�뵽�α��Ӧ��ƫ�Ƶ�ַ
    fread(sec_headers, sizeof(Elf64_Shdr), elf_header.e_shnum, fp);//���α����ݶ������ٵ�����sec_headers��

    int str_tab_ind = elf_header.e_shstrndx;//��ȡ�ַ�����.shstrtab�ڶα��е�����
    fseek(fp, sec_headers[str_tab_ind].sh_offset, SEEK_SET);//�ƶ�ָ�뵽�ַ�����.shstrtab��Ӧ��ƫ�Ƶ�ַ
    char* string_table = new char[sec_headers[str_tab_ind].sh_size];//���ٶ��ڴ���������ַ�����.shstrtab
    fread(string_table, 1, sec_headers[str_tab_ind].sh_size, fp);//���ַ�����.shstrtab��Ӧ��ַ�������ݶ����ַ���������

    int dynsym_ind = -1;//Ĭ��.dynsym���ű�����Ϊ-1
    int symtab_ind = -1;//Ĭ��.symtab���ű�����Ϊ-1
    int dynstr_ind = -1;//Ĭ��.dynstr�ַ���������Ϊ-1
    int strtab_ind = -1;//Ĭ��.strtab�ַ�������Ϊ-1

    //�����α�section_headers��ȡ���ű�.dynsym;.symtab;.dynstr;.strtab���ű��ڶα��е�����
    for (int i = 0; i < elf_header.e_shnum; i++) {
        if (sec_headers[i].sh_type == SHT_DYNSYM)//��.dynsym���ű�
            dynsym_ind = i;
        else if (sec_headers[i].sh_type == SHT_SYMTAB)//��.symtab���ű�
            symtab_ind = i;
        if (strcmp(&string_table[sec_headers[i].sh_name], ".strtab") == 0)//��.strtab�ַ�����
            strtab_ind = i;
        else if (strcmp(&string_table[sec_headers[i].sh_name], ".dynstr") == 0)//��.dynstr�ַ�����
            dynstr_ind = i;
    }
    //��ȡ���ű�entry��Ӧ��st_info�Σ���������������ͺͰ���Ϣ
    auto get_st_info = [](unsigned int st_info, string& symbol_type, string& symbol_binding) {
        unsigned char st_type = st_info & 0x0000000f;//��4λ��ʾ��������
        unsigned char st_binding = (st_info & (~0x0000000f)) >> 4;//��28λ��ʾ���Ű���Ϣ
        switch (st_binding) {
            case STB_LOCAL:
                symbol_binding = "LOCAL";
                break;
            case STB_GLOBAL:
                symbol_binding = "GLOBAL";
                break;
            case STB_WEAK:
                symbol_binding = "WEAK";
                break;
            case STB_NUM:
                symbol_binding = "NUM";
                break;
            case STB_LOOS:
                symbol_binding = "LOOS";
                break;
            case STB_HIOS:
                symbol_binding = "HIOS";
                break;
            case STB_LOPROC:
                symbol_binding = "LOPROC";
                break;
            case STB_HIPROC:
                symbol_binding = "HIPROC";
                break;
            default:
                symbol_binding = "UnknownType";
        }
        switch (st_type) {
            case STT_NOTYPE:
                symbol_type = "NOTYPE";
                break;
            case STT_OBJECT:
                symbol_type = "OBJECT";
                break;
            case STT_FUNC:
                symbol_type = "FUNC";
                break;
            case STT_SECTION:
                symbol_type = "SECTION";
                break;
            case STT_FILE:
                symbol_type = "FILE";
                break;
            case STT_COMMON:
                symbol_type = "COMMON";
                break;
            case STT_TLS:
                symbol_type = "TLS";
                break;
            case STT_NUM:
                symbol_type = "NUM";
                break;
            case STT_LOOS:
                symbol_type = "LOOS";
                break;
            case STT_HIOS:
                symbol_type = "HIOS";
                break;
            case STT_LOPROC:
                symbol_type = "LOPROC";
                break;
            case STT_HIPROC:
                symbol_type = "HIPROC";
                break;
            default:
                symbol_type = "UnknownBinding";
        }
    };

    //��ȡ�������ڵĶ��ڶα������������������Ž������⴦��
    auto get_st_shndx = [](unsigned int st_shndx, string& Ndx) {
        switch (st_shndx) {
            case SHN_UNDEF:
                Ndx = "UNDEF";
                break;
            case SHN_COMMON:
                Ndx = "COMMON";
                break;
            case SHN_ABS:
                Ndx = "ABS";
                break;
            default:
                Ndx = to_string(st_shndx);
        }
    };

    //������ű���Ϣ��������ű��ڶα��е�����sym_ind�����ű�entry��Ŀentry_num�����ű��Ӧ���ַ�����string_table
    auto show_symbol_table = [&](int sym_ind, unsigned long entry_num, char* string_table) {
        fseek(fp, sec_headers[sym_ind].sh_offset, SEEK_SET);//��ָ���ƶ������ű��Ӧ��ƫ�Ƶ�ַ

        Elf64_Sym* sym_entries = new Elf64_Sym[entry_num];//���ٶ��ڴ������洢���ű�������entry
        fread(sym_entries, sizeof(Elf64_Sym), entry_num, fp);//�����ű�
        printf("  Num:\t\tValue\t\tSize\tType\tBind\tVis\t"
               "Ndx\t\tName\n");
        //�������ű����ÿ��entry���������entry����Ϣ
        for (int i = 0; i < entry_num; i++) {
            printf("  %3d:\t", i);
            printf("0x%016lx:\t", sym_entries[i].st_value);
            printf("%4ld\t", sym_entries[i].st_size);
            string symbol_type;
            string symbol_binding;
            get_st_info(sym_entries[i].st_info, symbol_type, symbol_binding);
            printf("%s\t", symbol_type.data());
            printf("%s\t", symbol_binding.data());
            printf("DEFAULT\t");
            string Ndx;
            get_st_shndx(sym_entries[i].st_shndx, Ndx);
            printf("%4s\t", Ndx.data());
            //����entry��st_name�����ڷ��ű��Ӧ���ַ���������ҵ�entry��name
            printf("%s", &string_table[sym_entries[i].st_name]);
            printf("\n");
        }
        //�ͷŶ��ڴ�
        delete[] sym_entries;
    };

    //���.dynsym�δ���,��.dynstr����
    if ((dynsym_ind != -1) && (dynstr_ind != -1)) {
        //���ű��Сsec_headers[dynsym_ind].sh_sizeÿ��entry��Сsec_headers[dynsym_ind].sh_entsize
        // ����entry��Ŀentry_num
        unsigned long entry_num = sec_headers[dynsym_ind].sh_size / sec_headers[dynsym_ind].sh_entsize;
        printf("Symbol table '.dynsym' contains %ld entries\n", entry_num);
        fseek(fp, sec_headers[dynstr_ind].sh_offset, SEEK_SET);//��ָ���ƶ���.dynstr�ַ������Ӧ��ƫ�Ƶ�ַ
        //���ٶ��ڴ������洢�ַ�����
        char* dynstr_string_table = new char[sec_headers[dynstr_ind].sh_size];
        //�����ݶ����ַ�������
        fread(dynstr_string_table, 1, sec_headers[dynstr_ind].sh_size, fp);
        show_symbol_table(dynsym_ind, entry_num, dynstr_string_table);
        //�ͷ��ַ�����
        delete[] dynstr_string_table;
    } else {
        printf("No Dynamic linker symbol table!\n");
    }
    printf("\n");
    //���.symtab�δ��ڣ���.strtab����
    if ((symtab_ind != -1) && (strtab_ind != -1)) {
        unsigned long entry_num = sec_headers[symtab_ind].sh_size / sec_headers[symtab_ind].sh_entsize;
        printf("Symbol table '.symtab' contains %ld entries\n", entry_num);
        fseek(fp, sec_headers[strtab_ind].sh_offset, SEEK_SET);
        char* strtab_string_table = new char[sec_headers[strtab_ind].sh_size];
        fread(strtab_string_table, 1, sec_headers[strtab_ind].sh_size, fp);
        show_symbol_table(symtab_ind, entry_num, strtab_string_table);
        delete[] strtab_string_table;
    } else {
        printf("No symbol table!\n");
    }

    //�ͷ�
    delete[] string_table;
    delete[] sec_headers;
    fclose(fp);
}
