
#include <iostream>
#include <SFML/Graphics.hpp>
using namespace std;

#define ROW 31
#define COL 60  //Recommend ODD
#define MINE_NUM 250
#define FIXEL_SZ 32
#define WIDTH COL*FIXEL_SZ
#define HEIGHT (ROW+1)*FIXEL_SZ
#define GRAY 16
#define GREEN 11
#define MINE 9
#define BOOM 15
#define FLAG 10
#define QUE 13
#define FACE 12

class TILE {
private:
	sf::Texture t;
	sf::Sprite s;
public:
	TILE();
	void draw(sf::RenderWindow& _window, int x, int y, int posx, int posy);
};
TILE::TILE() {
	t.loadFromFile("images/tiles.png");
	s = sf::Sprite(t);
}
void TILE::draw(sf::RenderWindow& _window, int x, int y, int posx, int posy) {
	s.setPosition(x * FIXEL_SZ, y * FIXEL_SZ);
	s.setTextureRect(sf::IntRect(posx * FIXEL_SZ, posy* FIXEL_SZ, FIXEL_SZ, FIXEL_SZ));
	_window.draw(s);
}

class FUNC {
private:
	TILE tile = TILE();
	int menu[COL] = { 0 };
	sf::Clock clock;

public:
	bool timer_flag = 0;
	FUNC();
	void startTimer();
	void endTimer();
	void resetTimer();
	void updateFace(bool a);
	void showSetting(sf::RenderWindow& _window);
	void updateFlagLeft(int num);
};
FUNC::FUNC() {
	for (int i = 0; i < COL; i++) {
		menu[i] = 10;
	}
	for (int i = COL / 2 - 3; i <= COL / 2 + 3; i++) {
		menu[i] = 0;
	}
	menu[COL / 2] = FACE;
}
void FUNC::startTimer() {
	sf::Time elapsed = clock.getElapsedTime();
	int t = elapsed.asSeconds();
	for (int i = 0; i < 3; i++) {
		menu[COL / 2 - 3 + i] = 0;
	}
	int i = 0;
	while (t != 0) {
		i--;
		menu[COL/2 + i] = t % 10;
		t /= 10;
	}
}

void FUNC::endTimer() {}
void FUNC::resetTimer() {
	clock.restart();
}
void FUNC::updateFace(bool a){
	menu[COL / 2] = (a == true) ? 12 : 13;
}
void FUNC::showSetting(sf::RenderWindow& _window) {
	(timer_flag==true) ? startTimer() : endTimer();
	for (int j = 0; j < COL; j++) {
		int pos = menu[j];
		tile.draw(_window, j, 0, pos, 1);
	}
}
void FUNC::updateFlagLeft(int num) {
	int i = 0;
	for (int i = COL/2 + 1; i < COL / 2 + 4; i++) {
		menu[i] = 0;
	}
	while (num != 0) {
		menu[COL / 2 + 3 + i] = num % 10;
		num /= 10;
		i--;
	}
}
class MineSweeper : public FUNC {
private:
	TILE tile = TILE();
	int board[ROW][COL] = { 0 };
	int user_board[ROW][COL] = { 0 };
	int game_board[ROW][COL] = { 0 };
	int revealed = 0;
public:
	void reset();
	void placeMine();
	bool isMine(int x, int y);
	void surroundingCount(int x, int y);
	void addSurroundCount();
	bool isReveal(int x, int y);
	void revealSurrounding(int x, int y);
	void showGameBoard(sf::RenderWindow& _window);
	void updateGameBoard();
	void boomAll();
	bool isFace(int x, int y);
	void click_left(int x, int y);
	void click_right(int x, int y);
	int checkFlagUserPressed();
	int checkMineAnswer();
	void click(int x, int y);
};
void MineSweeper::reset() {
	for (int i = 0; i < ROW; i++) {
		for (int j = 0; j < COL; j++) {
			board[i][j] = 0;
			user_board[i][j] = 0;
			game_board[i][j] = 0;
		}
	}
	placeMine();
	addSurroundCount();
	revealed = 0;
}
void MineSweeper::placeMine() {
	int count = 0;
	while (MINE_NUM > count) {
		int x = rand() % ROW;
		int y = rand() % COL;
		if (board[x][y] == 0) {
			board[x][y] = MINE;
			count++;
		}
	}
}
bool MineSweeper::isMine(int x, int y) {
	if (x >= 0 && x < ROW && y >= 0 && y < COL) {
		if (board[x][y] == MINE) return 1;
		else return 0;
	}
	return 0;
}
void MineSweeper::surroundingCount(int x, int y) {
	if (board[x][y] == MINE) return;
	int result = 0;
	if (isMine(x + 1, y)) result++;
	if (isMine(x - 1, y)) result++;
	if (isMine(x, y + 1)) result++;
	if (isMine(x, y - 1)) result++;
	if (isMine(x - 1, y - 1)) result++;
	if (isMine(x - 1, y + 1)) result++;
	if (isMine(x + 1, y - 1)) result++;
	if (isMine(x + 1, y + 1)) result++;
	board[x][y] = result;
}
void MineSweeper::addSurroundCount() {
	for (int i = 0; i < ROW; i++) {
		for (int j = 0; j < COL; j++) {
			surroundingCount(i, j);
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
void MineSweeper::revealSurrounding(int x, int y) {
	if (user_board[x][y] == 1) return;
	user_board[x][y] = 1;
	revealed++;
	if (board[x][y] == 0) {
		if (isReveal(x - 1, y)) revealSurrounding(x - 1, y);
		if (isReveal(x + 1, y)) revealSurrounding(x + 1, y);
		if (isReveal(x, y - 1)) revealSurrounding(x, y - 1);
		if (isReveal(x, y + 1)) revealSurrounding(x, y + 1);
		if (isReveal(x + 1, y + 1)) revealSurrounding(x + 1, y + 1);
		if (isReveal(x - 1, y + 1)) revealSurrounding(x - 1, y + 1);
		if (isReveal(x - 1, y - 1)) revealSurrounding(x - 1, y - 1);
		if (isReveal(x + 1, y - 1)) revealSurrounding(x + 1, y - 1);
	}
}
void MineSweeper::showGameBoard(sf::RenderWindow& _window) {
	for (int i = 0; i < ROW; i++) {
		for (int j = 0; j < COL; j++) {
			int pos = game_board[i][j];
			if (user_board[i][j] == 0) pos = GREEN;
			tile.draw(_window, j, i+1, pos, 0);
		}
	}
}
void MineSweeper::updateGameBoard() {
	for (int i = 0; i < ROW; i++) {
		for (int j = 0; j < COL; j++) {
			if (user_board[i][j] == 0 || user_board[i][j] == 1) {
				game_board[i][j] = board[i][j] * user_board[i][j];
			}
			else {
				game_board[i][j] = user_board[i][j];
			}
		}
	}
}
void MineSweeper::boomAll() {
	for (int i = 0; i < ROW; i++) {
		for (int j = 0; j < COL; j++) {
			if (board[i][j] == MINE) {
				user_board[i][j] = BOOM;
			}
		}
	}
}
bool MineSweeper::isFace(int x, int y) {
	return ((x == 0) && (y == COL / 2)) ? 1 : 0;
}
void MineSweeper::click_left(int x, int y) {	
	//셋팅창
	if (y == 0) {
		if (isFace(y, x)) {
			reset();
			resetTimer();
			updateFace(true);
			timer_flag = true;
		}
	}
	//보드창
	else {
		//플래그가 라면,
		if (user_board[y - 1][x] == FLAG) {}
		//지뢰를 밟았을때, 
		else if (isMine(y - 1, x)) {
			cout << "Boom" << endl;
			boomAll();
			updateFace(false);
			timer_flag = false;
		}
		//지뢰를 밟지 않았고
		else {
			revealSurrounding(y - 1, x);
			if (timer_flag == false) {//처음시작일경우
				resetTimer();
				updateFace(true);
				timer_flag = true;
			}
		}
	}
}
void MineSweeper::click_right(int x, int y) {
	//셋팅창
	if (y == 0) {}
	//보드창
	else {
		if (user_board[y - 1][x] == 0 && (MINE_NUM - checkFlagUserPressed() > 0)) 
			user_board[y - 1][x] = FLAG; //처음 플래그
		else if (user_board[y - 1][x] == FLAG) user_board[y - 1][x] = QUE; //물음표
		else if (user_board[y - 1][x] == QUE) user_board[y - 1][x] = 0; //원래돌아오기
	}
}
int MineSweeper::checkMineAnswer() {
	int count = 0;
	for (int i = 0; i < ROW; i++) {
		for (int j = 0; j < COL; j++) {
			if ((user_board[i][j] == FLAG) && (board[i][j] == MINE)) {
				count++;
			}
		}
	}
	return count;
}
int MineSweeper::checkFlagUserPressed() {
	int count = 0;
	for (int i = 0; i < ROW; i++) {
		for (int j = 0; j < COL; j++) {
			if (user_board[i][j] == FLAG) {
				count++;
			}
		}
	}
	return count;
}
void MineSweeper::click(int x, int y) {
	//다풀었는지를 확인
	if ((revealed + checkMineAnswer()) == (ROW * COL)) {
		cout << "WIN" << endl;
		timer_flag = false;
	}
	//마인개수를 업데이트
	updateFlagLeft(MINE_NUM - checkFlagUserPressed());
}

int main() {
	sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Minesweeper");
	window.setFramerateLimit(15);

	sf::Image icon;
	icon.loadFromFile("images/bomb.png");
	window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

	MineSweeper game;
	game.placeMine();
	game.addSurroundCount();

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			int pos_x, pos_y;
			switch (event.type) {
			case sf::Event::Closed:
				window.close();
				break;
			case sf::Event::MouseButtonPressed:
				pos_x = event.mouseButton.x / FIXEL_SZ;
				pos_y = event.mouseButton.y / FIXEL_SZ;
				if (event.mouseButton.button == sf::Mouse::Left) {
					game.click_left(pos_x, pos_y);
				}
				else if (event.mouseButton.button == sf::Mouse::Right) {
					game.click_right(pos_x, pos_y);
				}
				game.click(pos_x, pos_y);
				game.updateGameBoard();
				break;
			default:
				window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
				break;
			}
		}
		window.clear();
		game.showSetting(window);
		game.showGameBoard(window);
		window.display();
	}
}