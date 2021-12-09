#include "header.h"

void readelf_s(const char* filename) {
    FILE* fp;
    Elf64_Ehdr elf_header;
    fp = fopen(filename, "r");
    if (fp == NULL) { exit(0); }
    fread(&elf_header, sizeof(Elf64_Ehdr), 1, fp);
    if (elf_header.e_ident[0] != 0x7f || elf_header.e_ident[1] != 'E') { exit(0); }
    Elf64_Shdr* sec_headers = new Elf64_Shdr[elf_header.e_shnum];//存放每个section_header的数组
    fseek(fp, elf_header.e_shoff, SEEK_SET);//移动指针到段表对应的偏移地址
    fread(sec_headers, sizeof(Elf64_Shdr), elf_header.e_shnum, fp);//将段表数据读到开辟的数组sec_headers里

    int str_tab_ind = elf_header.e_shstrndx;//获取字符串表.shstrtab在段表中的索引
    fseek(fp, sec_headers[str_tab_ind].sh_offset, SEEK_SET);//移动指针到字符串表.shstrtab对应的偏移地址
    char* string_table = new char[sec_headers[str_tab_ind].sh_size];//开辟堆内存用来存放字符串表.shstrtab
    fread(string_table, 1, sec_headers[str_tab_ind].sh_size, fp);//将字符串表.shstrtab对应地址处的数据读到字符串数组里

    int dynsym_ind = -1;//默认.dynsym符号表索引为-1
    int symtab_ind = -1;//默认.symtab符号表索引为-1
    int dynstr_ind = -1;//默认.dynstr字符串表索引为-1
    int strtab_ind = -1;//默认.strtab字符串索引为-1

    //遍历段表section_headers获取符号表.dynsym;.symtab;.dynstr;.strtab四张表在段表中的索引
    for (int i = 0; i < elf_header.e_shnum; i++) {
        if (sec_headers[i].sh_type == SHT_DYNSYM)//是.dynsym符号表
            dynsym_ind = i;
        else if (sec_headers[i].sh_type == SHT_SYMTAB)//是.symtab符号表
            symtab_ind = i;
        if (strcmp(&string_table[sec_headers[i].sh_name], ".strtab") == 0)//是.strtab字符串表
            strtab_ind = i;
        else if (strcmp(&string_table[sec_headers[i].sh_name], ".dynstr") == 0)//是.dynstr字符串表
            dynstr_ind = i;
    }
    //获取符号表entry对应的st_info段，用来计算符号类型和绑定信息
    auto get_st_info = [](unsigned int st_info, string& symbol_type, string& symbol_binding) {
        unsigned char st_type = st_info & 0x0000000f;//低4位表示符号类型
        unsigned char st_binding = (st_info & (~0x0000000f)) >> 4;//高28位表示符号绑定信息
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

    //获取符号所在的段在段表的索引，并对特殊符号进行特殊处理
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

    //输出符号表信息，输入符号表在段表中的索引sym_ind，符号表entry数目entry_num，符号表对应的字符串表string_table
    auto show_symbol_table = [&](int sym_ind, unsigned long entry_num, char* string_table) {
        fseek(fp, sec_headers[sym_ind].sh_offset, SEEK_SET);//将指针移动到符号表对应的偏移地址

        Elf64_Sym* sym_entries = new Elf64_Sym[entry_num];//开辟堆内存用来存储符号表中所有entry
        fread(sym_entries, sizeof(Elf64_Sym), entry_num, fp);//读符号表
        printf("  Num:\t\tValue\t\tSize\tType\tBind\tVis\t"
               "Ndx\t\tName\n");
        //遍历符号表里的每个entry，并且输出entry的信息
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
            //根据entry的st_name属性在符号表对应的字符串表表里找到entry的name
            printf("%s", &string_table[sym_entries[i].st_name]);
            printf("\n");
        }
        //释放堆内存
        delete[] sym_entries;
    };

    //如果.dynsym段存在,且.dynstr存在
    if ((dynsym_ind != -1) && (dynstr_ind != -1)) {
        //符号表大小sec_headers[dynsym_ind].sh_size每个entry大小sec_headers[dynsym_ind].sh_entsize
        // 计算entry数目entry_num
        unsigned long entry_num = sec_headers[dynsym_ind].sh_size / sec_headers[dynsym_ind].sh_entsize;
        printf("Symbol table '.dynsym' contains %ld entries\n", entry_num);
        fseek(fp, sec_headers[dynstr_ind].sh_offset, SEEK_SET);//将指针移动到.dynstr字符串表对应的偏移地址
        //开辟堆内存用来存储字符串表
        char* dynstr_string_table = new char[sec_headers[dynstr_ind].sh_size];
        //将数据读到字符串表里
        fread(dynstr_string_table, 1, sec_headers[dynstr_ind].sh_size, fp);
        show_symbol_table(dynsym_ind, entry_num, dynstr_string_table);
        //释放字符串表
        delete[] dynstr_string_table;
    } else {
        printf("No Dynamic linker symbol table!\n");
    }
    printf("\n");
    //如果.symtab段存在，且.strtab存在
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

    //释放
    delete[] string_table;
    delete[] sec_headers;
    fclose(fp);
}
