#pragma once
#include "header.h"
long get_rom_size(char* rom_name)
{
	FILE* fptr = fopen(rom_name, "rb");
	if (fptr == nullptr)
	{
		perror("fopen");
		exit(-1);
	}
	fseek(fptr, 0, SEEK_END);//fptr_end指向文件尾部
	long rom_size = ftell(fptr);
	fclose(fptr);
	return rom_size;
}

void load_rom(unsigned char*& memory,char* rom_name,long rom_size)
{
	FILE* fptr = fopen(rom_name, "rb");
	unsigned char* start = memory + 0x200;//绝大多数Chip-8程序从0x200地址处开始加载
	fread(start, sizeof(memory[0]), rom_size, fptr);
	fclose(fptr);
}

