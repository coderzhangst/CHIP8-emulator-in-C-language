#pragma once
#include "header.h"
#define ROM_START 0x200
void print_rom_memory(unsigned char* memory, long rom_size, char* rom_name)
{
	printf("rom size = % ld byte\n", rom_size);
	printf("%s content:\n", rom_name);
	int rom_index = ROM_START;//绝大多数Chip-8程序从0x200地址处开始加载
	int bound = ROM_START + rom_size;
	printf("offset 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n");
	printf("       -----------------------------------------------\n");
	printf("0x200: ");
	while (rom_index < bound)
	{
		    //memory数组元素的类型必须为unsigned char,stackoverflow上的回答: Why does printf not print out just one byte when printing hex?
			printf("%02X ", memory[rom_index++]);
			if (rom_index % 16==0)
				printf("\n0x%03X: ",rom_index);
	}
}

void print_fontset_memory(unsigned char* memory)
{
	int index = 0;
	printf("\nfontset_memory:\n");
	printf("offset 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n");
	printf("       -----------------------------------------------\n");
	printf("0x000: ");
	while (index < 80)
	{
		printf("%02X ", memory[index++]);
		if (index % 16 == 0)
			printf("\n0x%03X: ", index);
	}
}

void printf_cpu_and_devices()
{
	printf("cpu and devices:\n");
	for (int index = 0; index <= 0xF; index++)
	{
		printf("V[%X] = 0x%X ", index, V[index]);
		if (index % 4 == 3)
			printf("\n");
	}
	printf("DT = %d ", DT);
	printf("ST = %d ", ST);
	printf("SP = %d ", SP);
	printf("I = 0x%X ", I);
	printf("PC = 0x%X\n", PC);
}