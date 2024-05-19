#include <iostream>
#include <vector>
#include <random>
#include <string>
#include <SDL.h>

const bool debugMode = true;

void message(std::string message) {
	if (debugMode) std::cout << message << "\n";
}

int randomIntBetween(int a, int b) {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> distr(a, b);
	return distr(gen);
}

struct SnakePart {
	int posX;
	int posY;
	Uint32 col;

	SnakePart(int x, int y, Uint32 c) : posX(x), posY(y), col(c) {}
};

struct Food {
	int posX;
	int posY;

	Food(int x, int y) : posX(x), posY(y) {}
};

class Game {
private:
	SDL_Window* window = NULL;
	SDL_Surface* surface = NULL;

	SDL_Event event;

	Uint32 ticks;

	const int fps = 7;

	const int boardSizeX = 15;
	const int boardSizeY = 11;

	const int squareSize = 60;

	const int gameWidth = this->boardSizeX * this->squareSize;
	const int gameHeight = this->boardSizeY * this->squareSize;

	Uint32 headColor;
	Uint32 bodyColor1;
	Uint32 bodyColor2;
	Uint32 foodColor;
	Uint32 boardColor1;
	Uint32 boardColor2;

	std::vector<SnakePart> snake;
	std::vector<Food> foods;

	bool isRunning = false;

	int direction = 1;

	int score = 0;

	void limitFps() {
		if ((1000 / this->fps) > SDL_GetTicks() - this->ticks) {
			SDL_Delay(1000 / this->fps - (SDL_GetTicks() - this->ticks));
		}
	}

	void drawPixel(int x, int y, Uint32 color) {
		if (x >= 0 && y >= 0 && x < this->gameWidth && y < this->gameHeight) {
			Uint32* p = (Uint32*)this->surface->pixels + y * this->surface->w + x;
			*p = color;
		}
	}

	void handleEvents() {
		int prevDirection = this->direction;
		while (SDL_PollEvent(&this->event)) {
			if (this->event.type == SDL_KEYDOWN) {
				switch (this->event.key.keysym.sym) {
				case SDLK_w:
				case SDLK_i:
					if (prevDirection != 2) direction = 0;
					break;
				case SDLK_a:
				case SDLK_j:
					if (prevDirection != 1) direction = 3;
					break;
				case SDLK_s:
				case SDLK_k:
					if (prevDirection != 0) direction = 2;
					break;
				case SDLK_d:
				case SDLK_l:
					if (prevDirection != 3) direction = 1;
					break;
				}
			}
			else if (this->event.type == SDL_QUIT) isRunning = false;
		}
	}

	void drawBoard() {
		for (int i = 0; i < this->boardSizeX; i++) {
			for (int j = 0; j < this->boardSizeY; j++) {
				for (int k = 0; k < this->squareSize * this->squareSize; k++) {
					drawPixel(
						(i * this->squareSize) + (k % this->squareSize),
						(j * this->squareSize) + (k / this->squareSize),
						(i + j)%2 == 0 ? this->boardColor1 : this->boardColor2 );
				}
			}
		}
	}

	void drawSnake() {
		for (const SnakePart snakePart : this->snake) {
			for (int k = 0; k < this->squareSize * this->squareSize; k++) {
				drawPixel(
					snakePart.posX * this->squareSize + k % this->squareSize,
					snakePart.posY * this->squareSize + k / this->squareSize,
					snakePart.col);
			}
		}
	}

	void drawFood() {
		for (const Food food : this->foods) {
			for (int x = 0 - (this->squareSize / 2); x < this->squareSize / 2; x++) {
				for (int y = 0 - (this->squareSize / 2); y < this->squareSize / 2; y++) {
					if ((x * x) + (y * y) <= (this->squareSize / 2) * (this->squareSize / 2))
						drawPixel(
							food.posX * this->squareSize + this->squareSize / 2 + x,
							food.posY * this->squareSize + this->squareSize / 2 + y,
							this->foodColor);
				}
			}
		}
	}

	void moveSnake() {
		for (int i = snake.size() - 1; i > 0; i--) {
			this->snake[i].posX = this->snake[i - 1].posX;
			this->snake[i].posY = this->snake[i - 1].posY;
		}

		switch (direction) {
		case 0:
			this->snake[0].posY--;
			message("Changed snake direction: UP");
			break;
		case 1:
			this->snake[0].posX++;
			message("Changed snake direction: RIGHT");
			break;
		case 2:
			this->snake[0].posY++;
			message("Changed snake direction: DOWN");
			break;
		case 3:
			this->snake[0].posX--;
			message("Changed snake direction: LEFT");
			break;
		}

		// this part is responsible for looping mode (the snake teleporting thru walls)
		if (this->snake[0].posX == this->boardSizeX) this->snake[0].posX = 0;
		else if (this->snake[0].posX == -1) this->snake[0].posX = this->boardSizeX - 1;
		else if (this->snake[0].posY == this->boardSizeY) this->snake[0].posY = 0;
		else if (this->snake[0].posY == -1) this->snake[0].posY = this->boardSizeY - 1;
	}

	bool isSnakeThere(int x, int y) {
		for (SnakePart snakePart : snake) {
			if (x == snakePart.posX && y == snakePart.posY) return true;
		}
		return false;
	}

	void createFood() {
		int x, y;
		do {
			x = randomIntBetween(0, this->boardSizeX - 1);
			y = randomIntBetween(0, this->boardSizeY - 1);
			message("Creating random food position");
		} while (isSnakeThere(x, y));
		this->foods.push_back(Food(x, y));
		message("Food created");
	}

	void checkIfEaten() {
		for (int i = 0; i < this->foods.size(); i++) {
			if (this->foods[i].posX == this->snake[0].posX && this->foods[i].posY == this->snake[0].posY) {
				message("Food eaten");
				this->score++;
				std::string newTitle = "Snake Game in c++ & SDL2!  |  score: " + std::to_string(score);
				const char* newTitleChar = newTitle.c_str();
				SDL_SetWindowTitle(this->window, newTitleChar);
				this->snake.push_back(SnakePart(
					this->snake[this->snake.size() - 1].posX,
					this->snake[this->snake.size() - 1].posY,
					(this->snake.size() % 2) ? this->bodyColor1 : this->bodyColor2));
				this->foods.erase(this->foods.begin() + i);
				this->createFood();
			}
		}
	}

	bool checkCollisions() {
		for (int i = 1; i < this->snake.size(); i++) {
			if (this->snake[i].posX == this->snake[0].posX &&
				this->snake[i].posY == this->snake[0].posY) {
				message("Collision detected");
				return true;
			}
		}
		return false;
	}

	void loop() {
		while (this->isRunning) {
			this->ticks = SDL_GetTicks();
			this->handleEvents();
			this->moveSnake();
			if (this->checkCollisions()) break;
			this->checkIfEaten();
			this->drawBoard();
			this->drawFood();
			this->drawSnake();
			SDL_UpdateWindowSurface(this->window);
			this->limitFps();
		}
		while (this->isRunning) {
			this->ticks = SDL_GetTicks();
			this->handleEvents();
			this->limitFps();
		}
	}

public:
	Game() {
		if (SDL_Init(SDL_INIT_VIDEO) != 0) {
			std::cerr << "SDL could not be initialized. SDL error:\n" << SDL_GetError();
			return;
		}

		this->window = SDL_CreateWindow(
			"Snake Game in c++ & SDL2!  |  score: 0",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			this->gameWidth,
			this->gameHeight,
			SDL_WINDOW_SHOWN);
		if (this->window == nullptr) {
			std::cerr << "Window could not be created. SDL error:\n" << SDL_GetError();
			return;
		}

		this->surface = SDL_GetWindowSurface(this->window);
		if (this->surface == nullptr) {
			std::cerr << "Surface error. SDL error:\n" << SDL_GetError();
			return;
		}

		this->boardColor1	= SDL_MapRGB(this->surface->format, 35, 35, 35);
		this->boardColor2	= SDL_MapRGB(this->surface->format, 30, 30, 30);
		this->headColor		= SDL_MapRGB(this->surface->format, 0, 90, 0);
		this->bodyColor1	= SDL_MapRGB(this->surface->format, 80, 200, 80);
		this->bodyColor2	= SDL_MapRGB(this->surface->format, 100, 255, 100);
		this->foodColor		= SDL_MapRGB(this->surface->format, 200, 80, 80);

		this->snake.push_back(SnakePart(this->boardSizeX / 2, this->boardSizeY / 2, this->headColor));
		this->snake.push_back(SnakePart(this->boardSizeX / 2 - 1, this->boardSizeY / 2, this->bodyColor1));
		this->snake.push_back(SnakePart(this->boardSizeX / 2 - 2, this->boardSizeY / 2, this->bodyColor2));

		this->createFood();

		this->isRunning = true;
		this->loop();

		SDL_DestroyWindow(window);
		SDL_Quit();
	}

	~Game() {}
};

int main(int argc, char* args[])
{
	{
		Game* game = new Game();
	} // autmatically calls Game destructor ~Game()

	return 0;
}
