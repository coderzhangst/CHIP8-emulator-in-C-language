#pragma once
#include "header.h"
void printf_cpu_and_devices();
static unsigned char chip8_fontset[80] =
{
  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
  0x20, 0x60, 0x20, 0x20, 0x70, // 1
  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

unsigned char V[0xF + 1];//general purpose registers
unsigned short PC;//Program Counter
unsigned char SP;//Stack Pointer
unsigned short I;
unsigned char DT;//Delay Timer
unsigned char ST;//Sound Timer
bool key_state[0xF + 1];//按键状态://0表示未按下,1表示按下
unsigned short stack[16];// 最多递归16层
unsigned char   video_memory[32][64];//64*32像素,这里一个字符代表一个字节
void init_cpu_and_devices()
{
	//初始化寄存器
	memset(V, 0, sizeof(unsigned char) * 16);
	DT = 0;
	ST = 0;
	SP = 0;//定义:入栈SP++,出栈SP--
	PC = 0x200;

	//初始化栈段
	memset(stack, 0, sizeof(unsigned short) * 16);

	//初始化键盘状态
	memset(key_state, 0, sizeof(unsigned char) * 0xF);
}

void load_chip8_fontset(unsigned char* memory)
{
	//从0x000处开始加载
	for (short index = 0; index < 80; index++)
		memory[index] = chip8_fontset[index];
}

void unknown_opcode(unsigned short opcode, unsigned short PC)
{
	wchar_t buffer[35];
	swprintf(buffer, 35, L"Unknown opcode: 0x%X,PC=0x%X", opcode,PC);
	MessageBoxW(NULL,buffer, L"ERROR", MB_ICONERROR | MB_OK);
}

void beep()
{
	Beep(1000, 100);
}

unsigned char randbyte()
{
	std::uniform_int_distribution<unsigned int> distribution(0, 0xFF);
	std::default_random_engine generator;
	return distribution(generator);
}

void clean_all_key_state(bool* key_state)
{
	for (int index = 0; index <= 0xF; index++)
	{
		key_state[index] = false;
	}
}
void set_key_state()
{
	memset(key_state, 0, sizeof(key_state));
	if (KEY_PRESS('1'))
	{
		key_state[1] = true;
	}
	if (KEY_PRESS('2'))
	{
		key_state[2] = true;
	}
	if (KEY_PRESS('3'))
	{
		key_state[3] = true;
	}
	if (KEY_PRESS('4'))
	{
		key_state[0xC] = true;
	}
	if (KEY_PRESS('Q'))
	{
		key_state[4] = true;
	}
	if (KEY_PRESS('W'))
	{
		key_state[5] = true;
	}
	if (KEY_PRESS('E'))
	{
		key_state[6] = true;
	}
	if (KEY_PRESS('R'))
	{
		key_state[0xD] = true;
	}
	if (KEY_PRESS('A'))
	{
		key_state[7] = true;
	}
	if (KEY_PRESS('S'))
	{
		key_state[8] = true;
	}
	if (KEY_PRESS('D'))
	{
		key_state[9] = true;
	}
	if (KEY_PRESS('F'))
	{
		key_state[0xE] = true;
	}
	if (KEY_PRESS('Z'))
	{
		key_state[0xA] = true;
	}
	if (KEY_PRESS('X'))
	{
		key_state[0] = true;
	}
	if (KEY_PRESS('C'))
	{
		key_state[0xB] = true;
	}
	if (KEY_PRESS('V'))
	{
		key_state[0xF] = true;
	}
}

void emulate_cycle(unsigned char* memory)
{
	//取得指令,需要将memory[PC]和memory[PC + 1]组合为unsigned short
	//memory[PC]为高8位,memory[PC + 1]为低8位,借助移位操作:先memory[PC] << 8,后将这个结果与memory[PC + 1]做逻辑或
	register unsigned short opcode = ((memory[PC] << 8) | memory[PC + 1]);
	//nnn:指的是0xnnn地址处, 一个n是4位, 3个n就是32位
	//addr : 指的是地址
	//x : 高字节的低4位
	//y : 低字节的高4位
	//kk : 整个操作码的低8位
	//n: 整个操作码的低4位
	register unsigned char x = (opcode >> 8) & 0x000F;
	register unsigned char y = (opcode >> 4) & 0x000F;
	register unsigned char n = opcode & 0x000F;
	register unsigned char kk = opcode & 0x00FF;
	register unsigned short nnn = opcode & 0x0FFF;
#ifdef DEBUG
	unknown_opcode(opcode, PC);
#endif
	//指令的功能,使用switch&case判断,大的case判断opcode的最高字节,粗略分类功能
	switch (opcode & 0xF000)//只看opcode & 0xF000的最高字节,其余字节用0填充
	{
	case 0x0000:
	{
		switch (kk)//00E0 - CLS和00EE - RET 
		{
		case 0x00E0:
		{
			memset(video_memory, 0, sizeof(video_memory));
			system("cls");
			//print_window(video_memory);
			PC += 2;
			break;
		}
		case 0x00EE:
		{
			PC = stack[SP];
			SP--;
			break;
		}
		default:
		{
			PC += 2;
			break;
		}
		}
		break;
	}
	case 0x1000://1nnn - JP addr
	{
		PC = nnn;
		break;
	}
	case 0x2000://2nnn - CALL addr 
	{
		//先保存返回地址:当前PC的下一条指令
		SP++;
		stack[SP] = PC + 2;
		///再执行例程
		PC = nnn;
		break;
	}
	case 0x3000://3xkk - SE Vx, byte 
	{
		if (V[x] == kk)
		{
			PC += 4;
		}
		else
		{
			PC += 2;
		}
		break;
	}
	case 0x4000://4xkk - SNE Vx, byte
	{
		if (V[x] != kk)
		{
			PC += 4;
		}
		else
		{
			PC += 2;
		}
		break;
	}
	case 0x5000://5xy0 - SE Vx, Vy
	{
		if (V[x] == V[y])
		{
			PC += 4;
		}
		else
		{
			PC += 2;
		}
		break;
	}
	case 0x6000://6xkk - LD Vx, byte
	{
		V[x] = kk;
		PC += 2;
		break;
	}
	case 0x7000://7xkk - ADD Vx, byte
	{
		V[x] = V[x] + kk;
		PC += 2;
		break;
	}
	case 0x8000:
	{
		switch (n)
		{
		case 0x0://8xy0 - LD Vx, Vy 
		{
			V[x] = V[y];
			PC += 2;
			break;
		}
		case 0x1://8xy1 - OR Vx, Vy
		{
			V[x] |= V[y];
			PC += 2;
			break;
		}
		case 0x2://8xy2 - AND Vx, Vy
		{
			V[x] = V[x] & V[y];
			PC += 2;
			break;
		}
		case 0x3://8xy3 - XOR Vx, Vy 
		{
			V[x] ^= V[y];
			PC += 2;
			break;
		}
		case 0x4://8xy4 - ADD Vx, Vy
		{
			V[0xF] = ((int)V[x] + (int)V[y]) > 255 ? 1 : 0;
			V[x] += V[y];
			PC += 2;
			break;
		}
		case 0x5://8xy5 - SUB Vx, Vy
		{
			V[0xF] = (V[x] > V[y]) ? 1 : 0;
			V[x] -= V[y];
			PC += 2;
			break;
		}
		case 0x6://8xy6 - SHR Vx {, Vy}
		{
			V[0xF] = V[x] & 0x1;
			V[x] >>= 1;
			PC += 2;
			break;
		}
		case 0x7:// 8xy7 - SUBN Vx, Vy
		{
			V[0xF] = (V[y] > V[x]) ? 1 : 0;
			V[x] = V[y] - V[x];
			PC += 2;
			break;
		}
		case 0xE://8xyE - SHL Vx {, Vy} 
		{
			V[0xF] = (V[x] >> 7) & 0x1;
			V[x] <<= 1;
			PC += 2;
			break;
		}
		default:
		{
			unknown_opcode(opcode, PC);
			break;
		}
		}
		break;
	}
	case 0x9000://9xy0 - SNE Vx, Vy
	{
		if (V[x] != V[y])
		{
			PC += 4;
		}
		else
		{
			PC += 2;
		}
		break;
	}
	case 0xA000://Annn - LD I, addr
	{
		I = nnn;
		PC += 2;
		break;
	}
	case 0xB000:
	{
		PC = nnn + V[0];
		break;
	}
	case 0xC000://Cxkk - RND Vx, byte
	{
		V[x] = randbyte() & kk;
		PC += 2;
		break;
	}
	case 0xD000://Dxyn - DRW Vx, Vy, nibble
	{
		draw_sprite(I,V[x], V[y], n, memory,V,video_memory);
		PC += 2;
		break;
	}
	case 0xE000:
	{
		switch (kk)
		{
		case 0x9E://Ex9E - SKP Vx
		{
			if (key_state[V[x]])
			{
				PC += 4;
				break;
			}
			PC += 2;
			break;
		}
		case 0xA1://ExA1 - SKNP Vx
		{
			if (!key_state[V[x]])
			{
				PC += 4;
				break;
			}
			PC += 2;
			break;
		}
		default:
		{
			unknown_opcode(opcode, PC);
			break;
		}
		}
		break;
	}
	case 0xF000:
	{
		switch (kk)
		{
		case 0x07://Fx07 - LD Vx, DT 
		{
			V[x] = DT;
			PC += 2;
			break;
		}
		case 0x0A://Fx0A - LD Vx, K
		{
#ifdef DEBUG
			printf("Wait for key instruction\n");
#endif
			while (1) //一直检测键盘
			{
				set_key_state();
				for (int i = 0; i <= 0xF; i++)
				{
					if (key_state[i])
					{
						V[x] = i;
						goto label;
					}
				}
				Sleep(10);
			}
		label:
			PC += 2;
			break;
		}
		case 0x15://Fx15 - LD DT, Vx
		{
			DT = V[x];
			PC += 2;
			break;
		}
		case 0x18://Fx18 - LD ST, Vx
		{
			ST = V[x];
			PC += 2;
			break;
		}
		case 0x1E://Fx1E - ADD I, Vx
		{
			I = I + V[x];
			PC += 2;
			break;
		}
		case 0x29://Fx29 - LD F, Vx
		{
			I = V[x] * 0x05;
			PC += 2;
			break;
		}
		case 0x33://Fx33 - LD B, Vx 
		{
			memory[I] = (V[x] % 1000) / 100; //百位
			memory[I + 1] = (V[x] % 100) / 10;   // 十位
			memory[I + 2] = (V[x] % 10);         // 个位
			PC += 2;
			break;
		}
		case 0x55://Fx55 - LD[I], Vx
		{
			for (short index = 0; index <= x; index++)
				memory[I + index] = V[index];
			I += x + 1;
			PC += 2;
			break;
		}
		case 0x65://Fx65 - LD Vx, [I] 
		{
			for (short index = 0; index <= x; index++)
				V[index] = memory[I + index];
			I += x + 1;
			PC += 2;
			break;
		}
		}
		break;
	}
	default:
	{
		unknown_opcode(opcode, PC);
		break;
	}
	}
#ifdef  DEBUG
	printf_cpu_and_devices();
	getchar();
	system("cls");
#endif
}

//带打印调试信息的emulate_cycle 备份
//void emulate_cycle(unsigned char* memory)
//{
//	//取得指令,需要将memory[PC]和memory[PC + 1]组合为unsigned short
//	//memory[PC]为高8位,memory[PC + 1]为低8位,借助移位操作:先memory[PC] << 8,后将这个结果与memory[PC + 1]做逻辑或
//	unsigned short opcode = ((memory[PC] << 8) | memory[PC + 1]);
//	printf("PC = 0x%X 操作码: %X\n", PC, opcode);
//	//nnn:指的是0xnnn地址处, 一个n是4位, 3个n就是32位
//	//addr : 指的是地址
//	//x : 高字节的低4位
//	//y : 低字节的高4位
//	//kk : 整个操作码的低8位
//	//n: 整个操作码的低4位
//	unsigned char x = (opcode >> 8) & 0x000F;
//	unsigned char y = (opcode >> 4) & 0x000F;
//	unsigned char n = opcode & 0x000F;
//	unsigned short kk = opcode & 0x00FF;
//	unsigned int nnn = opcode & 0x0FFF;
//
//	printf("x=0x%X ,y=0x%X, n=0x%X, kk=0x%X ,nnn=0x%X\n", x, y, n, kk, nnn);
//	//指令的功能,使用switch&case判断,大的case判断opcode的最高字节,粗略分类功能
//	switch (opcode & 0xF000)//只看opcode & 0xF000的最高字节,其余字节用0填充
//	{
//	case 0x0000:
//	{
//		switch (kk)//00E0 - CLS和00EE - RET 
//		{
//		case 0x00E0:
//		{
//			system("cls");
//			printf("Clean the screen\n");
//			PC += 2;
//			break;
//		}
//		case 0x00EE:
//		{
//			PC = stack[SP];
//			printf("Return to 0x%X\n", PC);
//			SP--;
//			break;
//		}
//		default:
//		{
//			unknown_opcode(opcode, PC);
//			break;
//		}
//		}
//		break;
//	}
//	case 0x1000://1nnn - JP addr
//	{
//		PC = nnn;
//		printf("Jump to address 0x% X\n", nnn);
//		break;
//	}
//	case 0x2000://2nnn - CALL addr 
//	{
//		printf("Call address 0x %X\n", nnn);
//		//先保存返回地址:当前PC的下一条指令
//		SP++;
//		stack[SP] = PC + 2;
//		///再执行例程
//		PC = nnn;
//		break;
//	}
//	case 0x3000://3xkk - SE Vx, byte 
//	{
//		if (V[x] == kk)
//		{
//			printf("V%X == %X,Skip the next instruction\n", x, kk);
//			PC += 4;
//		}
//		else
//		{
//			printf("V%X != %X,Nothing to do\n", x, kk);
//			PC += 2;
//		}
//		break;
//	}
//	case 0x4000://4xkk - SNE Vx, byte
//	{
//		if (V[x] != kk)
//		{
//			printf("V%X != %X,Skip the next instruction\n", x, kk);
//			PC += 4;
//		}
//		else
//		{
//			printf("V%X == %X,Nothing to do\n", x, kk);
//			PC += 2;
//		}
//		break;
//	}
//	case 0x5000://5xy0 - SE Vx, Vy
//	{
//		if (V[x] == V[y])
//		{
//			printf("V%X == V%X,Skip the next instruction\n", x, y);
//			PC += 4;
//		}
//		else
//		{
//			printf("V%X != V%X,nothing to do\n", x, y);
//			PC += 2;
//		}
//		break;
//	}
//	case 0x6000://6xkk - LD Vx, byte
//	{
//		printf("V[%X] = 0x%X\n", x, kk);
//		V[x] = kk;
//		PC += 2;
//		break;
//	}
//	case 0x7000://7xkk - ADD Vx, byte
//	{
//		printf("Add 0x%X to V%X\n", kk, x);
//		V[x] = V[x] + kk;
//		PC += 2;
//		break;
//	}
//	case 0x8000:
//	{
//		switch (n)
//		{
//		case 0x0://8xy0 - LD Vx, Vy 
//		{
//			V[x] = V[y];
//			printf("V[%X] = V[%X]\n", x, y);
//			PC += 2;
//			break;
//		}
//		case 0x1://8xy1 - OR Vx, Vy
//		{
//			V[x] |= V[y];
//			printf("V[%X] |= V[%X]\n", x, y);
//			PC += 2;
//			break;
//		}
//		case 0x2://8xy2 - AND Vx, Vy
//		{
//			V[x] = V[x] & V[y];
//			printf("Set V%X = V%X AND V%X\n", x, x, y);
//			PC += 2;
//			break;
//		}
//		case 0x3://8xy3 - XOR Vx, Vy 
//		{
//			V[x] ^= V[y];
//			printf("V[%X] ^= V[%X],V[F] = %X\n", x, y, V[0xF]);
//			PC += 2;
//			break;
//		}
//		case 0x4://8xy4 - ADD Vx, Vy
//		{
//			V[x] += V[y];
//			V[0xF] = ((int)V[x] + (int)V[y]) > 255 ? 1 : 0;
//			printf("V[%X] += V[%X], V[F] = 0x%X\n", x, y, V[0xF]);
//			PC += 2;
//			break;
//		}
//		case 0x5://8xy5 - SUB Vx, Vy
//		{
//			V[0xF] = (V[x] > V[y]) ? 1 : 0;
//			V[x] -= V[y];
//			printf("V[%X] -= V[%X], V[F] =%X \n", x, y, V[0xF]);
//			PC += 2;
//			break;
//		}
//		case 0x6://8xy6 - SHR Vx {, Vy}
//		{
//			V[0xF] = V[x] & 0x1;
//			V[x] = V[x] >> 1;
//			printf("V[%X]>>1,V[F]=%X \n", x, V[0xF]);
//			PC += 2;
//			break;
//		}
//		case 0x7:// 8xy7 - SUBN Vx, Vy
//		{
//			V[0xF] = (V[y] > V[x]) ? 1 : 0;
//			V[x] = V[y] - V[x];
//			printf("V[%X] = V[%X] - V[%X],V[F] =%X ", x, y, x, V[0xF]);
//			PC += 2;
//			break;
//		}
//		case 0xE://8xyE - SHL Vx {, Vy} 
//		{
//			V[0xF] = (V[x] >> 7) & 0x1;
//			V[x] = V[x] << 1;
//			printf("V[%X] << 1,V[F] = %X", x, V[0xF]);
//			PC += 2;
//			break;
//		}
//		default:
//		{
//			unknown_opcode(opcode, PC);
//			break;
//		}
//		}
//		break;
//	}
//	case 0x9000://9xy0 - SNE Vx, Vy
//	{
//		if (V[x] != V[y])
//		{
//			printf("V%X != V%X,Skip the next instruction\n", x, y);
//			PC += 4;
//		}
//		else
//		{
//			printf("V%X == V%X,nothing to do\n", x, y);
//			PC += 2;
//		}
//	}
//	case 0xA000://Annn - LD I, addr
//	{
//		I = nnn;
//		printf("I = %X\n", nnn);
//		PC += 2;
//		break;
//	}
//	case 0xB000:
//	{
//		PC = nnn + V[0];
//		printf("Jump to  0x%X(nnn) + 0x%X(V[0])\n", nnn, V[0]);
//		break;
//	}
//	case 0xC000://Cxkk - RND Vx, byte
//	{
//		printf("V[%X] = randbyte() & %X\n", x, kk);
//		V[x] = randbyte() & kk;
//		PC += 2;
//		break;
//	}
//	case 0xD000://Dxyn - DRW Vx, Vy, nibble
//	{
//		printf("Draw sprite at (V[0x%x], V[0x%x]) = (0x%x, 0x%x) with %d bytes at the address stored in %X\n", x, y, V[x], V[y], n, I);
//		//draw_sprite(I,V[x], V[y], n, memory,video_memory);
//		PC += 2;
//		break;
//	}
//	case 0xE000:
//	{
//		switch (kk)
//		{
//		case 0x9E://Ex9E - SKP Vx
//		{
//			if (key_state[V[x]])
//			{
//				PC += 4;
//				printf("%X key is pressed,skip the next instruction\n", V[x]);
//				break;
//			}
//			printf("%X key isn't pressed,nothing to do\n", V[x]);
//			PC += 2;
//			break;
//		}
//		case 0xA1://ExA1 - SKNP Vx
//		{
//			if (!key_state[V[x]])
//			{
//				PC += 4;
//				printf("%X key isn't pressed,skip the next instruction\n", V[x]);
//				break;
//			}
//			printf("%X key is pressed,nothing to do\n", V[x]);
//			PC += 2;
//			break;
//		}
//		default:
//		{
//			unknown_opcode(opcode, PC);
//			break;
//		}
//		}
//		break;
//	}
//	case 0xF000:
//	{
//		switch (kk)
//		{
//		case 0x07://Fx07 - LD Vx, DT 
//		{
//			V[x] = DT;
//			printf("V%X = DT\n", x);
//			PC += 2;
//			break;
//		}
//		case 0x0A://Fx0A - LD Vx, K
//		{
//			printf("Wait for key instruction\n");
//			while (1) //一直检测键盘
//			{
//				for (int i = 0; i <= 0xF; i++)
//				{
//					if (key_state[i])
//					{
//						V[x] = i;
//						goto label;
//					}
//				}
//			}
//		label:
//			PC += 2;
//			break;
//		}
//		case 0x15://Fx15 - LD DT, Vx
//		{
//			DT = V[x];
//			printf("DT = V%X\n", x);
//			PC += 2;
//			break;
//		}
//		case 0x18://Fx18 - LD ST, Vx
//		{
//			ST = V[x];
//			printf("ST = V%X\n", x);
//			PC += 2;
//			break;
//		}
//		case 0x1E://Fx1E - ADD I, Vx
//		{
//			I = I + V[x];
//			printf("I = I + V%X\n", x);
//			PC += 2;
//			break;
//		}
//		case 0x29://Fx29 - LD F, Vx
//		{
//			I = V[x] * 0x05;
//			printf("I = V%X * 0x05\n", x);
//			PC += 2;
//			break;
//		}
//		case 0x33://Fx33 - LD B, Vx 
//		{
//			memory[I] = (V[x] % 1000) / 100; //百位
//			memory[I + 1] = (V[x] % 100) / 10;   // 十位
//			memory[I + 2] = (V[x] % 10);         // 个位
//			printf("Store BCD for V%X starting at address 0x%X\n", x, I);
//			PC += 2;
//			break;
//		}
//		case 0x55://Fx55 - LD[I], Vx
//		{
//			for (short index = 0; index <= 0xF; index++)
//				memory[I + index] = V[index];
//			printf("Copy sprite from V0 to VF  into memory at address 0x %X\n", I);
//			PC += 2;
//			break;
//		}
//		case 0x65://Fx65 - LD Vx, [I] 
//		{
//			for (short index = 0; index <= 0xF; index++)
//				V[index] = memory[I + index];
//			printf("Copy sprite from memory at address 0x%X into V0 to VF\n", I);
//			PC += 2;
//			break;
//		}
//		}
//		break;
//	}
//	default:
//	{
//		unknown_opcode(opcode, PC);
//		break;
//	}
//	}