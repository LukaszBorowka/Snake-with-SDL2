#include <iostream>
#include <vector>
#include <SDL.h>

struct SnakePart {
	int posX;
	int posY;
	Uint32 col;

	SnakePart(int x, int y, Uint32 c) : posX(x), posY(y), col(c) {}
};

class Game {
private:
	SDL_Window* window = NULL;
	SDL_Surface* surface = NULL;

	SDL_Event event;

	Uint32 ticks;

	const int fps = 8;

	const int boardSizeX = 15;
	const int boardSizeY = 15;

	const int squareSize = 40;

	const int gameWidth = this->boardSizeX * this->squareSize;
	const int gameHeight = this->boardSizeY * this->squareSize;

	Uint32 headColor;
	Uint32 bodyColor1;
	Uint32 bodyColor2;
	Uint32 foodColor;
	Uint32 boardColor1;
	Uint32 boardColor2;

	std::vector<SnakePart> snake;

	bool isRunning = false;

	int direction = 1;

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
					if (prevDirection != 2) direction = 0;
					break;
				case SDLK_a:
					if (prevDirection != 1) direction = 3;
					break;
				case SDLK_s:
					if (prevDirection != 0) direction = 2;
					break;
				case SDLK_d:
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
		for (const SnakePart snakePart : snake) {
			for (int k = 0; k < this->squareSize * this->squareSize; k++) {
				drawPixel(
					snakePart.posX * this->squareSize + k % this->squareSize,
					snakePart.posY * this->squareSize + k / this->squareSize,
					snakePart.col);
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
			break;
		case 1:
			this->snake[0].posX++;
			break;
		case 2:
			this->snake[0].posY++;
			break;
		case 3:
			this->snake[0].posX--;
		}

		if (this->snake[0].posX == this->boardSizeX) this->snake[0].posX = 0;
		if (this->snake[0].posX == -1) this->snake[0].posX = this->boardSizeX - 1;

		if (this->snake[0].posY == this->boardSizeY) this->snake[0].posY = 0;
		if (this->snake[0].posY == -1) this->snake[0].posY = this->boardSizeY - 1;
	}

	void loop() {
		while (isRunning) {
			this->ticks = SDL_GetTicks();
			this->drawBoard();
			this->drawSnake();
			SDL_UpdateWindowSurface(this->window);
			this->handleEvents();
			this->moveSnake();
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
			"Snake Game in c++ & SDL2!",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			this->gameWidth,
			this->gameHeight,
			SDL_WINDOW_SHOWN);
		if (this->window == NULL) {
			std::cerr << "Window could not be created. SDL error:\n" << SDL_GetError();
			return;
		}

		this->surface = SDL_GetWindowSurface(this->window);
		if (this->surface == NULL) {
			std::cerr << "Surface error. SDL error:\n" << SDL_GetError();
			return;
		}

		boardColor1 = SDL_MapRGB(this->surface->format, 40, 40, 40);
		boardColor2 = SDL_MapRGB(this->surface->format, 50, 50, 50);
		headColor = SDL_MapRGB(this->surface->format, 255, 100, 100);
		bodyColor1 = SDL_MapRGB(this->surface->format, 150, 150, 150);
		bodyColor2 = SDL_MapRGB(this->surface->format, 170, 170, 170);

		snake.push_back(SnakePart(this->boardSizeX / 2, this->boardSizeY / 2, this->headColor));
		snake.push_back(SnakePart(this->boardSizeX / 2 - 1, this->boardSizeY / 2, this->bodyColor1));
		snake.push_back(SnakePart(this->boardSizeX / 2 - 2, this->boardSizeY / 2, this->bodyColor2));

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
