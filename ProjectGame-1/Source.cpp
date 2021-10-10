#define _CRT_SECURE_NO_WARNINGS 1
#include<stdio.h>
#include<conio.h>
#include<stdlib.h>
#include<Windows.h>
#include<time.h>
#include<thread>
#include<math.h>
#include <SFML/Graphics.hpp>
#define scount 80
#define screen_x 80
#define screen_y 25

HANDLE wHnd;
HANDLE rHnd;
COORD bufferSize = { screen_x,screen_y };
COORD characterPos = { 0,0 };
COORD monster[scount];
COORD character;
COORD characterBullet[20];
COORD displayHp;
DWORD fdwMode;
SMALL_RECT windowSize = { 0,0,screen_x - 1,screen_y - 1 };
CHAR_INFO consoleBuffer[screen_x * screen_y];
#define PI 3.14159265

struct character_status {
	int hp = 5;
	int myArtifact = 0;
	int vectorX = 0;
	int vectorY = 0;
	int lookside = 2;
};

struct character_bullet {
	int bulletVectorX[20];
	int bulletVectorY[20];
	bool bulletState[20];
	int bulletDamage = 1;
	float bulletAngle[20];
};

struct artifact {
	int number = 0;
	int boostCharacterHp = 2;
	int boostCharacterSpeed = 1;
	float boostMonsterDropMoney = 0.5;
	int boostCharacterDamage = 1;
	int reduceMonsterDamage = 1;
	int reduceMonsterHp = 1;
	int boostMonsterDropMed = 1;
};

int setConsole(int x, int y) {
	wHnd = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleWindowInfo(wHnd, TRUE, &windowSize);
	SetConsoleScreenBufferSize(wHnd, bufferSize);
	return 0;
}

void clear_buffer() {
	for (int y = 0; y < screen_y; ++y) {
		for (int x = 0; x < screen_x; ++x) {
			consoleBuffer[x + screen_x * y].Char.AsciiChar = ' ';
			consoleBuffer[x + screen_x * y].Attributes = 7;
		}
	}
}

void fill_character_to_buffer() {
	consoleBuffer[character.X + screen_x * character.Y].Char.AsciiChar = 'X';
	consoleBuffer[character.X + screen_x * character.Y].Attributes = 7;
}

void fill_bullet_to_buffer(int j) {
	consoleBuffer[characterBullet[j].X + screen_x * characterBullet[j].Y].Char.AsciiChar = '*';
	consoleBuffer[characterBullet[j].X + screen_x * characterBullet[j].Y].Attributes = 6;
}

void fill_display_hp_to_buffer(int n) {
	for (int i = 0; i < n; i++) {
		consoleBuffer[displayHp.X + i + screen_x * displayHp.Y].Char.AsciiChar = 'O';
		consoleBuffer[displayHp.X + i + screen_x * displayHp.Y].Attributes = 4;
	}
}

void fill_buffer_to_console() {
	WriteConsoleOutputA(wHnd, consoleBuffer, bufferSize, characterPos, &windowSize);
}

int setMode() {
	rHnd = GetStdHandle(STD_INPUT_HANDLE);
	fdwMode = ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT;
	SetConsoleMode(rHnd, fdwMode);
	return 0;
}

void setcursor(bool visible)
{
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO lpCursor;
	lpCursor.bVisible = visible;
	lpCursor.dwSize = 20;
	SetConsoleCursorInfo(console, &lpCursor);
}

int main() {
	bool play = true;
	DWORD numEvents = 0;
	DWORD numEventsRead = 0;
	setConsole(screen_x, screen_y);
	setMode();
	setcursor(0);
	srand(time(NULL));
	character_status character_status;
	artifact artifact;
	character_bullet character_bullet;

	character.X = 20;
	character.Y = 20;
	while (play) {
		fill_display_hp_to_buffer(character_status.hp);
		fill_buffer_to_console();
		GetNumberOfConsoleInputEvents(rHnd, &numEvents);
		if (numEvents != 0) {
			INPUT_RECORD* eventBuffer = new INPUT_RECORD[numEvents];
			ReadConsoleInput(rHnd, eventBuffer, numEvents, &numEventsRead);
			for (DWORD i = 0; i < numEventsRead; ++i) {
				if (eventBuffer[i].EventType == KEY_EVENT && eventBuffer[i].Event.KeyEvent.bKeyDown == true) {
					if (eventBuffer[i].Event.KeyEvent.wVirtualKeyCode == VK_ESCAPE) {
						play = false;
						goto xxx;
					}
					if (eventBuffer[i].Event.KeyEvent.uChar.AsciiChar == 'e' || eventBuffer[i].Event.KeyEvent.uChar.AsciiChar == 'E') {
						character_status.myArtifact = artifact.number;
						character_status.hp += artifact.boostCharacterHp;
						character_bullet.bulletDamage *= artifact.boostCharacterDamage;

					}
					if (eventBuffer[i].Event.KeyEvent.uChar.AsciiChar == 'a' || eventBuffer[i].Event.KeyEvent.uChar.AsciiChar == 'A') {
						character_status.vectorX = -1;
					}
					if (eventBuffer[i].Event.KeyEvent.uChar.AsciiChar == 'd' || eventBuffer[i].Event.KeyEvent.uChar.AsciiChar == 'D') {
						character_status.vectorX = 1;
					}
					if (eventBuffer[i].Event.KeyEvent.uChar.AsciiChar == 'w' || eventBuffer[i].Event.KeyEvent.uChar.AsciiChar == 'W') {
						character_status.vectorY = -1;
					}
					if (eventBuffer[i].Event.KeyEvent.uChar.AsciiChar == 's' || eventBuffer[i].Event.KeyEvent.uChar.AsciiChar == 'S') {
						character_status.vectorY = 1;
					}
					character.X += character_status.vectorX;
					character.Y += character_status.vectorY;
				}
				else if (eventBuffer[i].EventType == MOUSE_EVENT) {
					int posx = eventBuffer[i].Event.MouseEvent.dwMousePosition.X;
					int posy = eventBuffer[i].Event.MouseEvent.dwMousePosition.Y;
					if (eventBuffer[i].Event.MouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) {
						for (int j = 0; j < 20; j++) {
							character_bullet.bulletState[j] = 1;
							characterBullet[j].X = character.X;
							characterBullet[j].Y = character.Y;
							character_bullet.bulletAngle[j] = (float)180/PI*atan2(abs(posy - character.Y) , abs(posx - character.X));
							if (character_bullet.bulletState[j] == 1) {
								if (character.X < posx) {
									if (character_bullet.bulletAngle[j] <= 45) {
										character_bullet.bulletVectorX[j] = 1;
										character_bullet.bulletVectorY[j] = 0;
									}
									else if (character_bullet.bulletAngle[j] > 45) {
										character_bullet.bulletVectorX[j] = 0;
										character_bullet.bulletVectorY[j] = -1;
									}
								}
								else if (character.X >= posx) {
									if (character_bullet.bulletAngle[j] <= 45) {
										character_bullet.bulletVectorX[j] = -1;
										character_bullet.bulletVectorY[j] = 0;
									}
									else if (character_bullet.bulletAngle[j] > 45) {
										character_bullet.bulletVectorX[j] = 0;
										character_bullet.bulletVectorY[j] = 1;
									}
								}
								characterBullet[j].X += character_bullet.bulletVectorX[j];
								characterBullet[j].Y += character_bullet.bulletVectorY[j];
								fill_bullet_to_buffer(j);
								fill_buffer_to_console();
							}
						}
					}
				}
			}
			delete[] eventBuffer;
		}
		fill_character_to_buffer();
		fill_buffer_to_console();
		clear_buffer();
		character_status.vectorX = 0;
		character_status.vectorY = 0;
		Sleep(100);
	}
xxx:
	system("cls");
	Beep(500, 100);
	printf("GAME OVER");
	return 0;
}