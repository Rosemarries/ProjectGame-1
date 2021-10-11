/*#define _CRT_SECURE_NO_WARNINGS 1
#include<stdio.h>
#include<conio.h>
#include<stdlib.h>
#include<Windows.h>
#include<time.h>
#include<thread>
#include<math.h>
#include<SFML/Graphics.hpp>
#define scount 80
#define screen_x 100
#define screen_y 40

#define room_x 80
#define room_y 25
#define room_start 5

HANDLE wHnd;
HANDLE rHnd;
COORD bufferSize = { screen_x,screen_y };
COORD characterPos = { 0,0 };
COORD monster[scount];
COORD character;
COORD characterBullet;
COORD displayHp;
COORD displayRoom;
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
	int bulletVectorX;
	int bulletVectorY;
	bool bulletState;
	int bulletDamage = 1;
	float bulletAngle;
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

void fill_character_to_buffer(int lookside) {
	if (lookside == 1) {
		consoleBuffer[character.X + screen_x * character.Y].Char.AsciiChar = 'R';
	}
	else if (lookside == 2) {
		consoleBuffer[character.X + screen_x * character.Y].Char.AsciiChar = 'D';
	}
	else if (lookside == 3) {
		consoleBuffer[character.X + screen_x * character.Y].Char.AsciiChar = 'L';
	}
	else if (lookside == 4) {
		consoleBuffer[character.X + screen_x * character.Y].Char.AsciiChar = 'U';
	}
	consoleBuffer[character.X + screen_x * character.Y].Attributes = 7;
}

void fill_bullet_to_buffer() {
	consoleBuffer[characterBullet.X + screen_x * characterBullet.Y].Char.AsciiChar = '*';
	consoleBuffer[characterBullet.X + screen_x * characterBullet.Y].Attributes = 6;
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

void draw_room() {
	for (int i = room_start; i < room_x + room_start; i++) {
		consoleBuffer[displayRoom.X + i + screen_x * (displayRoom.Y + room_start)].Char.AsciiChar = 'X';
		consoleBuffer[displayRoom.X + i + screen_x * (displayRoom.Y + room_start)].Attributes = 2;
		consoleBuffer[displayRoom.X + i + screen_x * (displayRoom.Y + room_start + room_y)].Char.AsciiChar = 'X';
		consoleBuffer[displayRoom.X + i + screen_x * (displayRoom.Y + room_start + room_y)].Attributes = 2;
	}
	for (int i = room_start; i < room_y + room_start; i++) {
		consoleBuffer[(displayRoom.Y + i) * screen_x + room_start].Char.AsciiChar = 'X';
		consoleBuffer[(displayRoom.Y + i) * screen_x + room_start].Attributes = 2;
		consoleBuffer[(displayRoom.Y + i) * screen_x + room_start + room_x - 1].Char.AsciiChar = 'X';
		consoleBuffer[(displayRoom.Y + i) * screen_x + room_start + room_x - 1].Attributes = 2;
	}
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
		draw_room();
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
						character_status.lookside = 3;
					}
					if (eventBuffer[i].Event.KeyEvent.uChar.AsciiChar == 'd' || eventBuffer[i].Event.KeyEvent.uChar.AsciiChar == 'D') {
						character_status.vectorX = 1;
						character_status.lookside = 1;
					}
					if (eventBuffer[i].Event.KeyEvent.uChar.AsciiChar == 'w' || eventBuffer[i].Event.KeyEvent.uChar.AsciiChar == 'W') {
						character_status.vectorY = -1;
						character_status.lookside = 4;
					}
					if (eventBuffer[i].Event.KeyEvent.uChar.AsciiChar == 's' || eventBuffer[i].Event.KeyEvent.uChar.AsciiChar == 'S') {
						character_status.vectorY = 1;
						character_status.lookside = 2;
					}
					character.X += character_status.vectorX;
					character.Y += character_status.vectorY;
					if (character.X <= room_start) {
						character.X = room_start + 1;
					}
					else if (character.X >= room_start + room_x - 1) {
						character.X = room_start + room_x - 2;
					}
					if (character.Y <= room_start) {
						character.Y = room_start + 1;
					}
					else if (character.Y >= room_start + room_y - 1) {
						character.Y = room_start + room_y - 1;
					}
				}
				else if (eventBuffer[i].EventType == MOUSE_EVENT) {
					int posx = eventBuffer[i].Event.MouseEvent.dwMousePosition.X;
					int posy = eventBuffer[i].Event.MouseEvent.dwMousePosition.Y;
					if (eventBuffer[i].Event.MouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) {
						character_bullet.bulletState = 1;
						characterBullet.X = character.X;
						characterBullet.Y = character.Y;
						character_bullet.bulletAngle = (float)180/PI*atan2(abs(posy - character.Y) , abs(posx - character.X));
						if (character_bullet.bulletState == 1) {
							if (character.X <= posx) {
								if (character_bullet.bulletAngle <= 45) {
									character_bullet.bulletVectorX = 1;
									character_bullet.bulletVectorY = 0;
								}
								else if (character_bullet.bulletAngle > 45) {
									if (character.Y < posy) {
										character_bullet.bulletVectorX = 0;
										character_bullet.bulletVectorY = 1;
									}
									else {
										character_bullet.bulletVectorX = 0;
										character_bullet.bulletVectorY = -1;
									}
								}
							}
							else if (character.X > posx) {
								if (character_bullet.bulletAngle <= 45) {
									character_bullet.bulletVectorX = -1;
									character_bullet.bulletVectorY = 0;
								}
								else if (character_bullet.bulletAngle > 45) {
									if (character.Y < posy) {
										character_bullet.bulletVectorX = 0;
										character_bullet.bulletVectorY = 1;
									}
									else {
										character_bullet.bulletVectorX = 0;
										character_bullet.bulletVectorY = -1;
									}
								}
							}
						}
					}
				}
			}
			delete[] eventBuffer;
		}
		if (character_bullet.bulletState == 1) {
			if (characterBullet.X <= room_start + 1 || characterBullet.X >= room_x + room_start - 2 || characterBullet.Y <= room_start + 1 || characterBullet.Y >= room_y + room_start - 1) {
				character_bullet.bulletState = 0;
			}
			else {
				characterBullet.X += character_bullet.bulletVectorX;
				characterBullet.Y += character_bullet.bulletVectorY;
				fill_bullet_to_buffer();
				fill_buffer_to_console();
			}
		}
		fill_character_to_buffer(character_status.lookside);
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
}*/