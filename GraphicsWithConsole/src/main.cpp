#include <iostream>
#include <thread>
#include <vector>
using namespace std;

#include <stdio.h>
#include <Windows.h>

enum Direction {
	UP = 1,
	DOWN = 2,
	LEFT = 3,
	RIGHT = 4
};

struct Piece {
	int x;
	int y;
	Direction dir;
	char ch;
};


const int nScreenWidth = 80;			// Console Screen Size X (columns)
const int nScreenHeight = 30;			// Console Screen Size Y (rows)
char scene[nScreenWidth * nScreenHeight] = { ' ' };
const int maxSnakeLength = 100;
const int snakeChar = 219;
const int foodChar = 178;
int snakeLength;
Piece snake[maxSnakeLength];
Piece head;

void hideConsoleCursor()
{
	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_CURSOR_INFO     cursorInfo;

	GetConsoleCursorInfo(out, &cursorInfo);
	cursorInfo.bVisible = false;
	SetConsoleCursorInfo(out, &cursorInfo);
}

void setColor(int text, int background) {

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	
	SetConsoleTextAttribute(hConsole, background * 16 + text);
}

void gotoXY(int x, int y) {

	COORD coord;
	coord.X = x;
	coord.Y = y;

	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void drawScene(Piece snake[]) {

	gotoXY(0, 0);
	
	setColor(12, 2);
	cout << scene;

	setColor(14, 2);
	for (int j = 0; j < snakeLength; j++)
	{
		int x = snake[j].x;
		int y = snake[j].y;
		gotoXY(x, y);
		cout << snake[j].ch;
	}
}

void readKeyboard(char keys[]) {

	for (int i = 0; i < 256; i++)
		keys[i] = (char)(GetAsyncKeyState(i) >> 8);
}

int growSnake() {

	if (snakeLength >= maxSnakeLength)
		return -1;

	snake[snakeLength].dir = snake[snakeLength - 1].dir;
	snake[snakeLength].x = snake[snakeLength - 1].x;
	snake[snakeLength].y = snake[snakeLength - 1].y;
	snake[snakeLength].ch = (char)snakeChar;

	switch (snake[snakeLength - 1].dir)
	{
	case UP:
		snake[snakeLength].y = (snake[snakeLength].y + 1) % nScreenHeight;
		break;
	case DOWN:
		snake[snakeLength].y = (snake[snakeLength].y - 1) % nScreenHeight;
		break;
	case LEFT:
		snake[snakeLength].x = (snake[snakeLength].x + 1) % nScreenWidth;
		break;
	case RIGHT:
		snake[snakeLength].x = (snake[snakeLength].x - 1) % nScreenWidth;
		break;
	}

	snakeLength++;
	return 0;
}

void synchDirections() {

	for (int i = snakeLength - 1; i > 0; i--)
	{
		snake[i].dir = snake[i - 1].dir;
	}
}

void setDirection(char keys[]) {

	if (keys[VK_UP] != 0 && snake[0].dir != DOWN)
		snake[0].dir = UP;
	if (keys[VK_DOWN] != 0 && snake[0].dir != UP)
		snake[0].dir = DOWN;
	if (keys[VK_LEFT] != 0 && snake[0].dir != RIGHT)
		snake[0].dir = LEFT;
	if (keys[VK_RIGHT] != 0 && snake[0].dir != LEFT)
		snake[0].dir = RIGHT;
	if (keys['G'] != 0)
		growSnake();
}

void moveSnake() {

	for (int i = 0; i < snakeLength; i++)
	{
		switch (snake[i].dir)
		{
		case UP:
			snake[i].y = (snake[i].y - 1 + nScreenHeight) % nScreenHeight;
			break;
		case DOWN:
			snake[i].y = (snake[i].y + 1 + nScreenHeight) % nScreenHeight;
			break;
		case LEFT:
			snake[i].x = (snake[i].x - 1 + nScreenWidth) % nScreenWidth;
			break;
		case RIGHT:
			snake[i].x = (snake[i].x + 1 + nScreenWidth) % nScreenWidth;
			break;
		}
	}

}

bool checkInSnake(int x, int y) {

	for (int i = 0; i < snakeLength; i++) {

		if (snake[i].x == x && snake[i].y == y)
			return true;
	}
	return false;
}

void addFood() {

	int x = rand() % nScreenWidth;
	int y = rand() % nScreenHeight;
	while (checkInSnake(x, y) != false) {

		x = rand() % nScreenWidth;
		y = rand() % nScreenHeight;
	}

	scene[y * nScreenWidth + x] = (char)foodChar;
}

int checkFoodHasEaten() {

	int x = snake[0].x;
	int y = snake[0].y;

	if (scene[y * nScreenWidth + x] == (char)foodChar) {

		scene[y * nScreenWidth + x] = ' ';
		return growSnake();
	}
	return 0;
}

void gameOver() {

	for (size_t i = 0; i < nScreenHeight * nScreenWidth; i++)
	{
		scene[i] = ' ';
	}

	gotoXY(0, 0);
	cout << scene;

	gotoXY(nScreenWidth / 2 - 10, nScreenHeight / 2 - 1);
	cout << "Game Over";
	gotoXY(nScreenWidth / 2 - 10, nScreenHeight / 2);
	if (snakeLength == maxSnakeLength)
		cout << "You won";
	else
		cout << "Your Score is " << snakeLength;

	cin.get();
}

bool isAlive() {

	for (int i = 1; i < snakeLength; i++) {

		if (snake[i].x == snake[0].x && snake[i].y == snake[0].y)
			return false;
	}
	return true;
}

void gameLoop(char keys[]) {

	int x = (rand() % nScreenWidth) / 5 + (nScreenWidth * 2 / 5);
	int y = (rand() % nScreenHeight) / 5 + (nScreenHeight * 2 / 5);

	snake[0].x = x;
	snake[0].y = y;
	snake[0].dir = (Direction)(rand() % 4 + 1);
	snake[0].ch = (char)snakeChar;
	snakeLength = 1;

	int i = 0;
	while (1) {

		readKeyboard(keys);
		setDirection(keys);
		if (i % 500 == 0)
			addFood();
		if (i % 15 == 0) {

			moveSnake();
			synchDirections();
		}

		if (checkFoodHasEaten() == -1)
			break;
		if (isAlive() != true)
			break;
		drawScene(snake);
		Sleep(10);
		i++;
	}
}


int main() {

	hideConsoleCursor();

	for (size_t i = 0; i < nScreenHeight * nScreenWidth; i++)
	{
		scene[i] = ' ';
	}

	char keys[256];

	srand(time(NULL));
	
	gameLoop(keys);

	gameOver();

	return 0;
}