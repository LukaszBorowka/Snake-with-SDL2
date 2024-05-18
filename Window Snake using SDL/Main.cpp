#include <iostream>
#include <vector>
#include <SDL.h>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

struct SnakePart {
	int posX;
	int posY;
	Uint32 col;
};

class Game {
private:
	SDL_Window* window = NULL;
	SDL_Surface* surface = NULL;

	SDL_Event event;

	const int gameWidth = 500;
	const int gameHeight = 500;

	std::vector<SnakePart> snakePart = {};

	bool isRunning = false;

public:
	Game() {
		if (SDL_Init(SDL_INIT_VIDEO) < 0) {
			std::cerr << "SDL could not be initialized. SDL error:\n" << SDL_GetError();
			exit;
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
			exit;
		}

		this->surface = SDL_GetWindowSurface(this->window);
		if (this->surface == NULL) {
			std::cerr << "Surface error. SDL error:\n" << SDL_GetError();
			exit;
		}

		SDL_FillRect(this->surface, NULL, SDL_MapRGB(this->surface->format, 0xFF, 0xFF, 0xFF));

		SDL_UpdateWindowSurface(this->window);

		this->isRunning = true;
		this->loop();
	}

	~Game() {
		SDL_DestroyWindow(window);
		SDL_Quit();
	}

	void loop() {
		while (isRunning) {


			while (SDL_PollEvent(&event)) {
				if (event.type == SDL_QUIT) isRunning = false;
			}
		}
	}
};

int main(int argc, char* args[])
{
	{
		Game* game = new Game();
	} // autmatically calls Game destructor ~Game()

	return 0;
}
