#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
using namespace std;

#define ROW 18 + 3
#define COL 14
#define MINE_NUM 8
#define BLOCK_SIZE 32
#define WIDTH (COL)*BLOCK_SIZE
#define HEIGHT (ROW)*BLOCK_SIZE

class TILE {
private:
	sf::Texture t;
	sf::Sprite s;
public:
	TILE();
	void draw(sf::RenderWindow& _window, int i, int j, int pos);
};
TILE::TILE() {
	t.loadFromFile("images/tiles.png");
	s = sf::Sprite(t);
}
void TILE::draw(sf::RenderWindow& _window, int x, int y, int pos) {
	s.setPosition((x) * 32, (y) * 32);
	s.setTextureRect(sf::IntRect(pos * 32, 0, 32, 32));
	_window.draw(s);
}

class MineSweeper {
private:
	int board[ROW][COL] = { 0 };
	int user_board[ROW][COL] = { 0 };
	int game_board[ROW][COL] = { 0 };
	int revealed = 0;
	TILE tile = TILE();

public:
	//----------- 보드 제작 -------------
	void placeMines(int _num_mine);
	bool isMine(int y, int x);
	void surroundingCount(int y, int x);
	void addSurroundingCount();

	//----------- 사용자 보드 ------------
	void showGameBoard(sf::RenderWindow& _window);
	bool isReveal(int y, int x);
	void surrondingReveal(int y, int x);

	//----------게임 ------------------
	void calcPosition(int& y, int& x);
	void bombAll();
	int checkMinsIsAnswer();
	void clickLeft(int y, int x);
	void clickRight(int y, int x);
	void updateGameBoard();

};
void MineSweeper::placeMines(int _num_mine) {
	int count = 0;
	while (_num_mine > count) {
		int x = rand() % ROW;
		int y = rand() % COL;
		if (board[x][y] == 0) {
			board[x][y] = 9;
			++count;
		}
	}
}
bool MineSweeper::isMine(int x, int y) {
	if (x >= 0 && x < ROW && y >= 0 && y < COL) {
		if (board[x][y] == 9) return 1;
		else return 0;
	}
	else return 0;
}
void MineSweeper::surroundingCount(int x, int y) {
	if (board[x][y] == 9) return;
	int result = 0;
	if (isMine(x + 1, y)) result++;
	if (isMine(x - 1, y)) result++;
	if (isMine(x, y - 1)) result++;
	if (isMine(x, y + 1)) result++;
	if (isMine(x + 1, y + 1)) result++;
	if (isMine(x + 1, y - 1)) result++;
	if (isMine(x - 1, y + 1)) result++;
	if (isMine(x - 1, y - 1)) result++;
	board[x][y] = result;
}
void MineSweeper::addSurroundingCount() {
	for (int i = 0; i < ROW; i++) {
		for (int j = 0; j < COL; j++) {
			surroundingCount(i, j);
		}
	}
}
void MineSweeper::showGameBoard(sf::RenderWindow& _window) {
	for (int i = 0; i < ROW; i++) {
		for (int j = 0; j < COL; j++) {
			int pos = game_board[i][j];
			if (user_board[i][j] == 0) pos = 16;
			tile.draw(_window, j, i, pos);
		}
	}
}
bool MineSweeper::isReveal(int x, int y) {
	if (x >= 0 && x < ROW && y >= 0 && y < COL) {
		if (user_board[x][y] == 0) {
			return 1;
		}
		else return 0;
	}
	else return 0;
}
void MineSweeper::surrondingReveal(int x, int y) {
	if (user_board[x][y] == 1) return;
	user_board[x][y] = 1;
	revealed++;
	if (board[x][y] == 0) {
		if (isReveal(x - 1, y)) surrondingReveal(x - 1, y);
		if (isReveal(x + 1, y)) surrondingReveal(x + 1, y);
		if (isReveal(x, y - 1)) surrondingReveal(x, y - 1);
		if (isReveal(x, y + 1)) surrondingReveal(x, y + 1);
		if (isReveal(x + 1, y + 1)) surrondingReveal(x + 1, y + 1);
		if (isReveal(x + 1, y - 1)) surrondingReveal(x + 1, y - 1);
		if (isReveal(x - 1, y + 1)) surrondingReveal(x - 1, y + 1);
		if (isReveal(x - 1, y - 1)) surrondingReveal(x - 1, y - 1);
	}
}
void MineSweeper::calcPosition(int& x, int& y) {
	x = (x / 32);
	y = (y / 32);
}
void MineSweeper::bombAll() {
	for (int i = 0; i < ROW; i++) {
		for (int j = 0; j < COL; j++) {
			if (board[i][j] == 9) {
				user_board[i][j] = 15;
			}
		}
	}
}
int MineSweeper::checkMinsIsAnswer() {
	int count = 0;
	for (int i = 0; i < ROW; i++) {
		for (int j = 0; j < COL; j++) {
			if ((user_board[i][j] == 10) && (board[i][j] == 9)) {
				count++;
			}
		}
	}
	return count;
}
void MineSweeper::clickLeft(int x, int y) {
	calcPosition(x, y);
	if (isMine(y, x)) {
		cout << "Boom" << endl;
		bombAll();
	}
	surrondingReveal(y, x);
	if (revealed + checkMinsIsAnswer() == ROW * COL) {
		cout << "Done" << endl;
	}
}
void MineSweeper::clickRight(int x, int y) {
	calcPosition(x, y);
	if (user_board[y][x] == 0) { //1
		user_board[y][x] = 10;
	}
	else if (user_board[y][x] == 10) { //2
		user_board[y][x] = 14;
	}
	else if (user_board[y][x] == 14) { //3
		user_board[y][x] = 0;
	}
}
void MineSweeper::updateGameBoard() {
	for (int i = 0; i < ROW; i++)
	{
		for (int j = 0; j < COL; j++)
		{
			if (user_board[i][j] == 0 || user_board[i][j] == 1) {
				game_board[i][j] = user_board[i][j] * board[i][j];
			}
			else {
				game_board[i][j] = user_board[i][j];
			}
		}
	}
}


int main() {
	sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Minesweeper");
	window.setFramerateLimit(15);

	MineSweeper game;
	game.placeMines(MINE_NUM);
	game.addSurroundingCount();


	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			cout << 1 << endl;
			switch (event.type) {
			case sf::Event::Closed:
				window.close();
				break;
			case sf::Event::MouseButtonPressed:
				if (event.mouseButton.button == sf::Mouse::Left) {
					cout << "x : " << event.mouseButton.x << endl;
					cout << "y : " << event.mouseButton.y << endl;
					game.clickLeft(event.mouseButton.x, event.mouseButton.y);
					game.updateGameBoard();
				}
				else if (event.mouseButton.button == sf::Mouse::Right) {
					game.clickRight(event.mouseButton.x, event.mouseButton.y);
					game.updateGameBoard();
				}
				break;
			default:
				cout << 2 << endl;
				break;
			}
		}
		window.clear();
		//easel.draw(window);
		game.showGameBoard(window);
		window.display();
	}
	return 0;
}