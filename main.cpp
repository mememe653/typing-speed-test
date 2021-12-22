#include <iostream>
#include <SDL.h>
#include <SDL_ttf.h>

bool init(SDL_Surface**, SDL_Window**);

int main(int argc, char** args) {

	SDL_Surface* winSurface = nullptr;
	SDL_Window* window = nullptr;

	if (!init(&winSurface, &window)) {
		system("pause");
		return 1;
	}

	SDL_Renderer* renderer;
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (!renderer) {
		std::cout << "Error creating renderer: " << SDL_GetError() << std::endl;
	}

	SDL_SetRenderDrawColor(renderer, 0, 255, 0, 127);
	SDL_RenderClear(renderer);

	if (TTF_Init() < 0) {
		std::cout << "Error initialising SDL_ttf: " << TTF_GetError() << std::endl;
	}

	TTF_Font* font;
	font = TTF_OpenFont("resources\\Lato-Black.ttf", 24);
	if (!font) {
		std::cout << "Failed to load font: " << TTF_GetError() << std::endl;
	}

	SDL_Color color = { 255, 0, 0 };
	SDL_Surface* text = TTF_RenderText_Solid(font, "Hello World!", color);
	if (!text) {
		std::cout << "Failed to render text: " << TTF_GetError() << std::endl;
	}

	SDL_Texture* textTexture;
	textTexture = SDL_CreateTextureFromSurface(renderer, text);
	if (text) {
		SDL_Rect dest = { 0, 0, text->w, text->h };
		SDL_RenderCopy(renderer, textTexture, nullptr, &dest);
	}

	SDL_RenderPresent(renderer);




	system("pause");

	// Destroy the window. This will also destroy the surface
	SDL_DestroyWindow(window);

	SDL_Quit();

	return 0;
}

bool init(SDL_Surface** surface, SDL_Window** window) {
	// SDL_Init will return -1 if it fails.
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		std::cout << "Error initializing SDL: " << SDL_GetError() << std::endl;
		return false;
	}

	*window = SDL_CreateWindow("Typing Speed Test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 720, SDL_WINDOW_SHOWN);

	if (!*window) {
		std::cout << "Error creating window: " << SDL_GetError() << std::endl;
		return false;
	}

	*surface = SDL_GetWindowSurface(*window);

	if (!*surface) {
		std::cout << "Error getting surface: " << SDL_GetError() << std::endl;
		return false;
	}

	// Fill the window with a white rectangle
	SDL_FillRect(*surface, NULL, SDL_MapRGB((*surface)->format, 255, 255, 255));

	SDL_UpdateWindowSurface(*window);

	return true;
}