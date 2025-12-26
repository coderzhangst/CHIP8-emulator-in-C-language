#include "header.h"
#define _CRT_SECURE_NO_WARNINGS
#define RAM_SIZE (0xFFF-0+1) //4KB
int  main(int argc, char* argv[])
{
	
	if (argc == 1)
	{
		printf("Usage: CHIP8.exe YourFilePath");
		exit(-1);
	}
	
	long rom_size=get_rom_size(argv[1]);
    unsigned char* memory = (unsigned char*)malloc(sizeof(unsigned char)* RAM_SIZE);
	
	load_rom(memory, argv[1], rom_size);
	init_window();
	init_cpu_and_devices(); 
	load_chip8_fontset(memory);
#ifdef DEBUG
	getchar();
	beep();
	print_rom_memory(memory,rom_size,argv[1]);
	print_fontset_memory(memory);
	printf_cpu_and_devices();
#endif
	while (1)
	{
		if (KEY_PRESS(VK_ESCAPE))
			break;

		set_key_state();
		emulate_cycle(memory);
		DT--;
		ST--;
	}
	free(memory);
	fflush(stdin); //Çå¿Õ stdin »º³åÇø
	system("cls");
	printf("Quit...");
	Sleep(500);
	return 0;
}