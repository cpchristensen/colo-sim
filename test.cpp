#include <iostream>
#include <SDL2/SDL.h>

using namespace std;

int main () {
	if (SDL_Init(SDL_INIT_EVERYTHING)) {
		cerr << "SDL failed to initialize." << endl;
		return 1;
	} else {
		cout << "SDL initialized." << endl;
	}

	SDL_Window* window;
	SDL_Renderer* renderer;
	if (SDL_CreateWindowAndRenderer(640, 480, 0, window, renderer)) {
		cerr << "Error creating window and renderer." << endl;
		return 1;
	}
	
	while (true) {
		SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
		SDL_RenderClear(renderer);

		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		SDL_RenderDrawPoint(renderer, 100, 100);

		SDL_RenderPresent(renderer);

		SDL_Event event;
		if (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				break;
			}
		}
	}


	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
