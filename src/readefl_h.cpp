#include "header.h"

void readelf_h(const char* filename) {
    FILE* fp;//定义文件指针
    Elf64_Ehdr elf_header;//定义elf头用来存储
    fp = fopen(filename, "r");
    if (fp == NULL) { exit(0); }
    fread(&elf_header, sizeof(Elf64_Ehdr), 1, fp);//读header
    if (elf_header.e_ident[0] != 0x7f || elf_header.e_ident[1] != 'E') { exit(0); }//判断是否是elf文件
    printf("ELF Header:\n");
    printf("  Magic:\t");
    for (unsigned char i : elf_header.e_ident) {
        printf("%02x ", i);
    }
    printf("\n");
    printf("  Class:\t\t\t\t");
    switch (elf_header.e_ident[EI_CLASS]) {
        case 1:
            printf("ELF%d\n", 32);
            break;
        case 2:
            printf("ELF%d\n", 64);
            break;
        default:
            printf("Error!\n");
    }
    printf("  Data:\t\t\t\t\t");
    switch (elf_header.e_ident[EI_DATA]) {
        case 1:
            printf("2's complement, little endian\n");
            break;
        case 2:
            printf("2's complement, big endian\n");
            break;
        default:
            printf("Error!\n");
    }
    printf("  Version:\t\t\t\t%d (current)\n", elf_header.e_ident[EI_VERSION]);
    printf("  OS/ABI:\t\t\t\t");
    switch (elf_header.e_ident[EI_OSABI]) {
        case ELFOSABI_NONE:
            printf("UNIX System V ABI\n");
            break;
        case ELFOSABI_HPUX:
            printf("HP-UX\n");
            break;
        case ELFOSABI_NETBSD:
            printf("NetBSD.\n");
            break;
        case ELFOSABI_GNU:
            printf("Object uses GNU ELF extensions.\n");
            break;
        case ELFOSABI_SOLARIS:
            printf("Sun Solaris.\n");
            break;
        case ELFOSABI_AIX:
            printf("IBM AIX.\n");
            break;
        case ELFOSABI_IRIX:
            printf("SGI Irix.\n");
            break;
        case ELFOSABI_FREEBSD:
            printf("FreeBSD.\n");
            break;
        case ELFOSABI_TRU64:
            printf("Compaq TRU64 UNIX.\n");
            break;
        case ELFOSABI_MODESTO:
            printf("Novell Modesto.\n");
            break;
        case ELFOSABI_OPENBSD:
            printf("OpenBSD.\n");
            break;
        case ELFOSABI_ARM_AEABI:
            printf("ARM EABI\n");
            break;
        case ELFOSABI_ARM:
            printf("ARM\n");
            break;
        case ELFOSABI_STANDALONE:
            printf("Standalone (embedded) application\n");
            break;
        default:
            printf("Error!\n");
    }
    printf("  ABI Version:\t\t\t\t%d\n", elf_header.e_ident[EI_ABIVERSION]);
    printf("  Type:\t\t\t\t\t");
    switch (elf_header.e_type) {
        case ET_REL:
            printf("REL (Relocatable file)\n");
            break;
        case ET_EXEC:
            printf("EXEC (Executable file)\n");
            break;
        case ET_DYN:
            printf("DYN (Shared object file)\n");
            break;
        default:
            printf("Error!\n");
    }
    printf("  Machine:\t\t\t\t");
    switch (elf_header.e_machine) {
        case EM_NONE:
            printf("No Machine!\n");
            break;
        case EM_386:
            printf("Intel 80386\n");
            break;
        case EM_860:
            printf("Intel 80860\n");
            break;
        case EM_ARM:
            printf("ARM\n");
            break;
        case EM_X86_64:
            printf("AMD x86-64 architecture\n");
            break;
        case EM_AVR:
            printf("Atmel AVR 8-bit microcontroller\n");
            break;
        case EM_MSP430:
            printf("Texas Instruments msp430\n");
            break;
        case EM_ALTERA_NIOS2:
            printf("Altera Nios II\n");
            break;
        case EM_MICROBLAZE:
            printf("Xilinx MicroBlaze\n");
            break;
        case EM_8051:
            printf("Intel 8051 and variants\n");
            break;
        case EM_STM8:
            printf("STMicroelectronics STM8\n");
            break;
        case EM_CUDA:
            printf("NVIDIA CUDA\n");
            break;
        case EM_AMDGPU:
            printf("AMD GPU\n");
            break;
        case EM_RISCV:
            printf("RISC-V\n");
            break;
        case EM_BPF:
            printf("Linux BPF -- in-kernel virtual machine\n");
            break;
        default:
            printf("Unknown Machine!\n");
    }
    printf("  Version:\t\t\t\t%x\n", elf_header.e_ident[EI_VERSION]);
    printf("  Entry point address:\t\t\t0x%016lx\n", elf_header.e_entry);
    printf("  Start of program headers:\t\t%ld (bytes into file)\n", elf_header.e_phoff);
    printf("  Start of section headers:\t\t%ld (bytes into file)\n", elf_header.e_shoff);
    printf("  Flags:\t\t\t\t0x%x\n", elf_header.e_flags);
    printf("  Size of this header:\t\t\t%d (bytes)\n", elf_header.e_ehsize);
    printf("  Size of program headers:\t\t%d (bytes)\n", elf_header.e_phentsize);
    printf("  Number of program headers:\t\t%d\n", elf_header.e_phnum);
    printf("  Size of section headers:\t\t%d (bytes)\n", elf_header.e_shentsize);
    printf("  Number of section headers:\t\t%d\n", elf_header.e_shnum);
    printf("  Section header string table index:\t%d\n", elf_header.e_shstrndx);
}
