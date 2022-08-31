#include <elf.h>
#include <stdio.h>
#include <string.h>

#define PHNUM 1
#define ENTRY 0x401000
#define FILE_OFF (sizeof(Elf64_Ehdr) + PHNUM * sizeof(Elf64_Phdr))

//程序入口在code数组中偏移
#define E_INCODE 13

//死循环代码，E_INCODE置零
// unsigned char code[] = { 0xeb, 0xfe };
//退出程序代码，E_INCODE置零
// unsigned char code[] = { 0xbf, 0x00, 0x00, 0x00, 0x00, 0xb8,
//			 0x3c, 0x00, 0x00, 0x00, 0x0f, 0x05 };
//输出字符串代码，E_INCODE置13
unsigned char code[] = { 0xe9, 0x80, 0xa0, 0xe5, 0x8f, 0x8d, 0xe6, 0x9c, 0x89,
			 0xe7, 0x90, 0x86, 0x0a, 0xb8, 0x01, 0x00, 0x00, 0x00,
			 0xbf, 0x01, 0x00, 0x00, 0x00, 0x48, 0xbe, 0x78, 0x10,
			 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0xba, 0x0d, 0x00,
			 0x00, 0x00, 0x0f, 0x05, 0xb8, 0x3c, 0x00, 0x00, 0x00,
			 0xbf, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x05 };

int main(void) {
	FILE *output = fopen("output", "wb");

	Elf64_Ehdr header;
	memset(&header, 0, sizeof(Elf64_Ehdr));
	unsigned char e_ident[EI_NIDENT] = { 0x7f, 0x45, 0x4c, 0x46,
					     0x02, 0x01, 0x01 };
	memcpy(header.e_ident, e_ident, EI_NIDENT);
	header.e_type = ET_EXEC;
	header.e_machine = EM_X86_64;
	header.e_version = 1;
	header.e_entry = ENTRY + FILE_OFF + E_INCODE;
	header.e_phoff = sizeof(Elf64_Ehdr);
	header.e_ehsize = sizeof(Elf64_Ehdr);
	header.e_phentsize = sizeof(Elf64_Phdr);
	header.e_phnum = PHNUM;

	fwrite(&header, sizeof(Elf64_Ehdr), 1, output);

	Elf64_Phdr phdr[PHNUM];
	memset(phdr, 0, sizeof(Elf64_Phdr) * PHNUM);

	phdr[0].p_type = PT_LOAD;
	phdr[0].p_flags = PF_X | PF_R;
	phdr[0].p_offset = 0x0;
	phdr[0].p_paddr = phdr[0].p_vaddr = ENTRY;
	phdr[0].p_filesz = phdr[0].p_memsz = sizeof(code) + FILE_OFF;
	phdr[0].p_align = 0x1000;

	fwrite(phdr, sizeof(Elf64_Phdr), 1, output);

	fwrite(code, sizeof(code), 1, output);

	fclose(output);
}
