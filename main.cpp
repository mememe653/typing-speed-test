#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <cstdlib>
#include <SDL.h>
#include <SDL_ttf.h>

int SCREEN_WIDTH = 1280;
int SCREEN_HEIGHT = 720;

bool init(SDL_Surface**, SDL_Window**);


class Word {
private:
	const char* word;
	int x, y, w, h;
	TTF_Font* font;
	SDL_Color color;
	int idx = 0;

public:
	Word(const char* word, int y, TTF_Font* font, SDL_Color color) : word(word), x(0), y(y), font(font), color(color) {
		SDL_Surface* text = TTF_RenderText_Solid(font, word, color);
		this->w = text->w;
		this->h = text->h;
	}

	void writeNextLetter(char letter) {
		if (word[idx] == letter) {
			color = { 255, 0, 0 };
			idx++;
		} else {
			idx = 0;
			color = { 255, 255, 0 };
		}
	}

	bool completed() {
		return idx >= strlen(word);
	}

	void render(SDL_Renderer* renderer) {
		SDL_Surface* text = TTF_RenderText_Solid(font, word, color);
		SDL_Texture* textTexture;
		textTexture = SDL_CreateTextureFromSurface(renderer, text);
		SDL_Rect dest = { x, y, text->w, text->h };
		SDL_RenderCopy(renderer, textTexture, nullptr, &dest);
	}

	void incrementX(int dx) {
		this->x += dx;
	}

	int getX() const {
		return this->x;
	}

	int getY() const {
		return this->y;
	}

	int getWidth() const {
		return this->w;
	}

	int getHeight() const {
		return this->h;
	}
};


class WordGenerator {
private:
	TTF_Font* font;
	SDL_Color color;
	std::string* possibleWords;

public:
	WordGenerator(TTF_Font* font, const SDL_Color& color)
		: font(font), color(color) {
		std::ifstream wordsFile("resources\\english_words.txt");
		possibleWords = new std::string[3000];
		for (int i = 0; i < 3000; i++) {
			std::getline(wordsFile, possibleWords[i]);
		}
	}

	void generate(std::vector<Word>& words) {
		Word newWord(possibleWords[rand() % 3000].c_str(), rand() % SCREEN_HEIGHT, font, color);
		for (Word& word : words) {
			if ((newWord.getX() + newWord.getWidth()) > word.getX()) {
				if (newWord.getY() < (word.getY() + word.getHeight())
					&& (newWord.getY() + newWord.getHeight()) > word.getY()) {
					return;
				}
			}
		}
		if ((newWord.getY() + newWord.getHeight()) < SCREEN_HEIGHT) {
			words.push_back(newWord);
		}
	}
};


int main(int argc, char** args) {

	srand(time(NULL));

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

	if (TTF_Init() < 0) {
		std::cout << "Error initialising SDL_ttf: " << TTF_GetError() << std::endl;
	}

	TTF_Font* font;
	font = TTF_OpenFont("resources\\Lato-Black.ttf", 24);
	if (!font) {
		std::cout << "Failed to load font: " << TTF_GetError() << std::endl;
	}

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);

	bool quit = false;
	int x = 0;
	SDL_Color wordColor = { 255, 255, 0 };
	SDL_Event e;

	WordGenerator wordGenerator(font, wordColor);
	std::vector<Word> words;

	int maxCount = 100;
	int generateCounter = maxCount;
	int wordCount = 0;
	auto startTime = std::chrono::high_resolution_clock::now();
	while (!quit) {
		if (generateCounter == 0) {
			generateCounter = maxCount > 10 ? maxCount-- : 10;
			wordGenerator.generate(words);
		}
		if (SDL_PollEvent(&e)) {
			if (e.type == SDL_KEYDOWN) {
				for (Word& word : words) {
					word.writeNextLetter(e.key.keysym.sym);
				}
			}
		}
		SDL_RenderClear(renderer);
		for (int i = 0; i < words.size(); i++) {
			if (words[i].completed()) {
				words.erase(words.begin() + i);
				wordCount++;
			}
			else {
				words[i].render(renderer);
				words[i].incrementX(1);
				if ((words[i].getX() + words[i].getWidth()) > SCREEN_WIDTH) {
					quit = true;
				}
			}
		}
		SDL_RenderPresent(renderer);
		SDL_Delay(10);
		generateCounter--;
	}

	auto endTime = std::chrono::high_resolution_clock::now();
	auto time = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
	int wpm = wordCount * 1000 * 60 / time;

	SDL_RenderClear(renderer);
	std::string gameOverMessage = std::string("Typing speed: ") + std::to_string(wpm) + " wpm";
	SDL_Color gameOverColor = { 255, 255, 0 };
	SDL_Surface* text = TTF_RenderText_Solid(font, gameOverMessage.c_str(), gameOverColor);
	SDL_Texture* textTexture;
	textTexture = SDL_CreateTextureFromSurface(renderer, text);
	SDL_Rect dest = { 0, 0, text->w, text->h };
	SDL_RenderCopy(renderer, textTexture, nullptr, &dest);
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

	*window = SDL_CreateWindow("Typing Speed Test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

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