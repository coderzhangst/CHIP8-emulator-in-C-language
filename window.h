#pragma once
#include "header.h"
//VK 是 Virtual-Key Code 的缩写,表示虚拟键码,https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
#define KEY_PRESS(VK)  ( (GetAsyncKeyState(VK) & 0x1) ? 1 : 0 )|| (GetAsyncKeyState(VK) & 0x8000) //判断一个键是否被按过，可以检测GetAsyncKeyState返回值的最低值是否为1
void set_pos(short x, short y)//设置光标的位置
{
	COORD pos = { x, y };
	HANDLE hOutput = NULL;
	hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(hOutput, pos);
}

void init_window()
{
	//隐藏光标
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO CursorInfo;
	GetConsoleCursorInfo(handle, &CursorInfo);//获取控制台光标信息
	CursorInfo.bVisible = false;
	SetConsoleCursorInfo(handle, &CursorInfo);

	system("title CHIP-8 Emulator by CSDN zhangcoder~ 按Esc可退出模拟器:-)");
	system("cls");
	setlocale(LC_ALL, "");//支持宽字符输出
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hConsole == INVALID_HANDLE_VALUE) 
	{
		printf("无法获取控制台句柄！\n");
		exit(GetLastError());
	}

	HWND hwnd = GetConsoleWindow();
	if (hwnd) {
		ShowWindow(hwnd, SW_MAXIMIZE);  // 最大化
	}
	
}

void gotoxy(int x, int y) 
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD pos = { (SHORT)x, (SHORT)y };
	SetConsoleCursorPosition(hConsole, pos);
}

inline void print_square() //实心方块
{
	wprintf(L"%lc", L'\u25A0');
}

inline void  print_whitespace()// 空格
{
	wprintf(L"%lc", L'\u3000'); 
}

static wchar_t screen[32 * 65];
void print_window(unsigned char video_memory[32][64])
{
	
	int index = 0;
	for (int x= 0; x < 32; x++) 
	{
		for (int y = 0; y < 64; y++) 
		{
			if (video_memory[x][y])
				screen[index++] = L'\u25A0';
			else
				screen[index++] = L'\u3000'; // 空格
		}
		screen[index++] = L'\n'; // 换行
	}
	set_pos(0, 0);
	DWORD charsWritten;
	static HANDLE hConsole = INVALID_HANDLE_VALUE;

	if (hConsole == INVALID_HANDLE_VALUE) 
	{
		hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	}
	WriteConsoleW(hConsole, screen, 32 * 65, &charsWritten, NULL);//x系统调用比一个一个打印字符要快
	//玩pong的推荐wait: 10000000
	//玩Breakout的推荐wait: 30000000
	volatile unsigned long long  wait = 10000000;//可以手动调整屏幕刷新速度,加上volatile是防止编译器优化掉无用的循环
	while (wait--);
}

void draw_sprite(unsigned short I,
	unsigned char  vx,
	unsigned char  vy,
	unsigned char  n,
	unsigned char* memory,
	unsigned char* V,
	unsigned char video_memory[32][64])
{
	unsigned row = vy, col = vx;
	unsigned byte_index;
	unsigned bit_index;

	V[0xF] = 0;
	for (byte_index = 0; byte_index < n; byte_index++) 
	{
		uint8_t byte = memory[I + byte_index];
		for (bit_index = 0; bit_index < 8; bit_index++) 
		{
			uint8_t bit = (byte >> (7 - bit_index)) & 0x1; 
			//uint8_t* pixelp = &(video_memory[(col + bit_index) % 64][(row + byte_index) % 32]);
			uint8_t* pixelp = &(video_memory[(row + byte_index) % 32][(col + bit_index) % 64]);

			if (bit == 1 && *pixelp == 1)
				V[0xF] = 1;

			*pixelp ^= bit;
		}
	}
	print_window(video_memory); 
}