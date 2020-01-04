#include <stdio.h>
#include <math.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <assert.h>

#define SEED time(NULL)

typedef struct {
	SDL_Point position;
	int civ;
	SDL_Color color;
	int strength;
	int movement_range;
	char sex;
	int age;
	int lifespan;
	int reproductive_value;
	int is_alive;
	size_t civ_index;
} Person;

int colorCompare (SDL_Color p1, SDL_Color p2) {
	return (p1.r == p2.r) && (p1.g == p2.g) && (p1.b == p2.b) && (p1.a == p2.a);
}

SDL_Point* mapGen (size_t width, size_t height, size_t land_area, int continents_n) {
	int x, y;

	SDL_Point* land_points = malloc(land_area * sizeof(SDL_Point));
	// x = rand() % width;
	// y = rand() % height;
	x = width / 2;
	y = height / 2;

	land_points[0].x = x;
	land_points[0].y = y;

	printf("Picked initial spot for continent generation.\n");

	//Used to determine if the given tile is occupied in one operation.
	int virtual_map[height][width];
	for (size_t i = 0; i < height; i++) {
		for (size_t j = 0; j < width; j++) {
			virtual_map[i][j] = 0;
		}
	}
	virtual_map[land_points[0].y][land_points[0].x] = 1;
	printf("Created virtual map.\n");

	int continent_increment = land_area / continents_n;
	for (size_t i = 1; i < land_area; i++) {
		if (i % continent_increment == 0) {
			x = rand() % width;
			y = rand() % height;
		} else {
			int x_offset = 0, y_offset = 0;
			if (rand() % 2) {
				x_offset = (rand() % 3) - 1;
			} else {
				y_offset = (rand() % 3) - 1;
			}

			x += x_offset;
			y += y_offset;

			if (x < 0 || x >= width || y < 0 || y >= height) {
				x -= x_offset;
				y -= y_offset;
			}
		}

		if (virtual_map[y][x]) {
			i--;
			continue;
		}

		land_points[i].x = x;
		land_points[i].y = y;
		virtual_map[y][x] = 1;
	}
	printf("Placed all lands.\n");

	return land_points;
}

int pointIsIn (SDL_Point p, SDL_Point* points, size_t size) {
	for (size_t i = 0; i < size; i++) {
		if (p.x == points[i].x && p.y == points[i].y) {
			return i;
		}
	}

	return -1;
}

int main () {
	srand(SEED);
	printf("Seed: %d\n", SEED);

	SDL_Color white = {255, 255, 255, 255};
	SDL_Color black = {0, 0, 0, 255};

	SDL_Color red = {255, 0, 0, 255};
	SDL_Color green = {0, 255, 0, 255};
	SDL_Color blue = {0, 0, 255, 255};

	SDL_Color purple = {255, 0, 255, 255};
	SDL_Color yellow = {255, 255, 0, 255};

	float frame_time;
	clock_t start;

	const size_t width = 800;
	const size_t height = 400;

	size_t land_area = (int) ((float) (width * height) * 0.5);
	land_area = 20000;
	int number_of_continents = 1;

	const size_t periods = 10000;

	if (SDL_Init(SDL_INIT_EVERYTHING)) {
		fprintf(stderr, "SDL failed to initialize.\n");
		return 1;

	}
	if (TTF_Init() == -1) {
		fprintf(stderr, "SDL_TTF failed to initialize.\n");
	}
	TTF_Font* font = TTF_OpenFont("courier-new.ttf", 16);

	
	SDL_Window* window = SDL_CreateWindow("SDL Test", 0, 0, width, height + 100, 0);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	printf("SDL initialized.\n");


	SDL_Point* land_points = mapGen(width, height, land_area, number_of_continents);
	SDL_Point* ocean_points = malloc(width * height * sizeof(SDL_Point));
	size_t ocean_points_index = 0;

	printf("Map generated.\n");

	size_t number_of_civs = 3;
	SDL_Color civ_colors[] = {red, white, black};
	// SDL_Color civ_colors[number_of_civs];
	// for (size_t i = 0; i < number_of_civs; i++) {
		// civ_colors[i] = (SDL_Color) {rand() % 256, rand() % 256, rand() % 256, 255};
	// }

	size_t people_size = 128;
	Person* people = malloc(people_size * sizeof(Person));
	size_t people_n = 0;

	SDL_Color virtual_map[width][height];
	for (size_t i = 0; i < width; i++) {
		for (size_t j = 0; j < height; j++) {
			virtual_map[i][j] = blue;
			ocean_points[ocean_points_index++] = (SDL_Point) {i, j};
		}
	}

	for (size_t i = 0; i < land_area; i++) {
		virtual_map[land_points[i].x][land_points[i].y] = green;
	}

	for (size_t i = 0; i < number_of_civs; i++) {
		int tmp = rand() % land_area;
		people[people_n].civ = i;
		people[people_n].color = civ_colors[i];
		people[people_n].position.x = land_points[tmp].x;
		people[people_n].position.y = land_points[tmp].y;

		people[people_n].age = 0;
		people[people_n].is_alive = 1;
		people[people_n].civ_index = 0;

		people[people_n].lifespan = 50 + (rand() % 20);
		people[people_n].sex = rand() % 2 ? 'm' : 'f';
		people[people_n].strength = rand() % 100;
		people[people_n].movement_range = 1;
		people_n++;

		virtual_map[land_points[tmp].x][land_points[tmp].y] = civ_colors[i];
	}

	SDL_Point* civ_points[number_of_civs];
	size_t civ_points_n[number_of_civs];
	for (size_t i = 0; i < number_of_civs; i++) {
		civ_points[i] = malloc(10000000 * sizeof(SDL_Point));
		civ_points_n[i] = 1;
		civ_points[i][0] = people[i].position;
	}

	int population = number_of_civs;
	int civ_pops[number_of_civs];
	for (size_t i = 0; i < number_of_civs; i++) {
		civ_pops[i] = 1;
	}

	printf("Beginning simulation.\n");
	for (size_t i = 0; i < periods; i++) {
		if (population == 0) break;

		start = clock();

		for (size_t j = 0; j < people_n; j++) {
			people[j].age++;

			if (people[j].age == people[j].lifespan && people[j].is_alive) {

				virtual_map[people[j].position.x][people[j].position.y] = green;
				people[j].position.x = people[j].position.y = -1;
				civ_points[people[j].civ][people[j].civ_index] = people[j].position;

				population--;
				civ_pops[people[j].civ]--;


				people[j].is_alive = 0;
			}

			if (people[j].age >= people[j].lifespan || !people[j].is_alive) continue;

			if (people[j].age > 13 && !(rand() % 20)) {

				SDL_Point current_pos = people[j].position;
				SDL_Point new_point = {-1, -1};
				for (size_t count = 0; count < 30; count++) {
					size_t k = current_pos.x;
					size_t l = current_pos.y;

					k += (rand() % 3) - 1;
					l += (rand() % 3) - 1;

					if (k == current_pos.x && l == current_pos.y) continue;
					if (k < 0 || l < 0 || k >= width || l >= height) continue;
					if (!colorCompare(virtual_map[k][l], green)) continue;

					new_point.x = k;
					new_point.y = l;

					break;
				}

				// If there is no available point around for new person, repeat turn.
				if (new_point.x == -1) {
					people[j].age--;
					j--;
					continue;
				}

				Person n_p;
				n_p.civ = people[j].civ;
				n_p.color = people[j].color;
				n_p.position = new_point;

				n_p.age = 0;
				n_p.is_alive = 1;
				n_p.civ_index = civ_points_n[n_p.civ];

				n_p.lifespan = 40 + (rand() % 20);
				n_p.sex = rand() % 2 ? 'm' : 'f';
				n_p.strength = people[j].strength + ((rand() % 20) - 5);
				n_p.movement_range = 1;

				int found_recycle = 0;
				int rec_index = 0;
				for (size_t k = 0; k < people_n; k++) {
					if (!people[k].is_alive && people[k].age > people[k].lifespan) {
						found_recycle = 1;
						rec_index = k;
					}
				}

				if (found_recycle) {
					people[rec_index] = n_p;
				} else {
					people[people_n++] = n_p;
					if (people_n == people_size) {
						people_size *= 2;
						people = realloc(people, people_size * sizeof(Person));
					}
				}


				civ_points[n_p.civ][civ_points_n[n_p.civ]++] = n_p.position;

				population++;
				civ_pops[n_p.civ]++;

				virtual_map[new_point.x][new_point.y] = n_p.color;

			// Movement.
			} else {
				SDL_Point current_pos = people[j].position;
				SDL_Point new_point = {-1, -1};

				for (size_t count = 0; count < 30; count++) {
					size_t k = current_pos.x;
					size_t l = current_pos.y;

					k += (rand() % 3) - 1;
					l += (rand() % 3) - 1;

					if (k == current_pos.x && l == current_pos.y) continue;
					if (colorCompare(virtual_map[k][l], blue)) continue;
					if (colorCompare(virtual_map[k][l], people[j].color)) continue;
					if (k < 0 || l < 0 || k >= width || l >= height) continue;

					new_point.x = k;
					new_point.y = l;

					break;
				}

				// If there is no available point around for new person, skip turn.
				if (new_point.x == -1) continue;

				int is_occupied = 0;
				int occupied_index = -1;

				for (size_t k = 0; k < number_of_civs; k++) {
					if (colorCompare(virtual_map[new_point.x][new_point.y], civ_colors[k])) {
						is_occupied = 1;
						for (size_t l = 0; l < people_n; l++) {
							if (people[l].position.x == new_point.x && people[l].position.y == new_point.y) {
								occupied_index = l;
							}
						}
						break;
					}
				}
				if (is_occupied) {
					assert(occupied_index > -1);
				}

				if (is_occupied && (people[occupied_index].strength >= people[j].strength)) {
					people[j].is_alive = 0;
					population--;

					civ_pops[people[j].civ]--;


					virtual_map[people[j].position.x][people[j].position.y] = green;

					people[j].position.x = people[j].position.y = -1;
					civ_points[people[j].civ][people[j].civ_index] = people[j].position;


				} else {
					virtual_map[people[j].position.x][people[j].position.y] = green;
					virtual_map[new_point.x][new_point.y] = people[j].color;

					people[j].position = new_point;
					civ_points[people[j].civ][people[j].civ_index] = people[j].position;

					if (is_occupied && (people[occupied_index].strength < people[j].strength)) {
						people[occupied_index].is_alive = 0;
						population--;

						civ_pops[people[occupied_index].civ]--;


						people[occupied_index].position.x = people[occupied_index].position.y = -1;
						civ_points[people[occupied_index].civ][people[occupied_index].civ_index] = people[occupied_index].position;
					}
				}

			}

		}

		frame_time = (float) (clock() - start) / (float) CLOCKS_PER_SEC;

		// Makes the window blue.
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
		SDL_RenderDrawPoints(renderer, ocean_points, width * height);

		// Switches color to green for land.
		SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
		SDL_RenderDrawPoints(renderer, land_points, land_area);

		for (size_t j = 0; j < number_of_civs; j++) {
			// Switches color for tiles of each civ.
			SDL_SetRenderDrawColor(renderer, civ_colors[j].r, civ_colors[j].g, civ_colors[j].b, civ_colors[j].a);

			SDL_RenderDrawPoints(renderer, civ_points[j], civ_points_n[j]);
		}

		int w, h;

		char* str = calloc(1000, 1);
		sprintf(str, "Population: %zu\nProgress: %.2lf%%\npeople_n: %zu\n", population, ((float) i / (float) periods) * 100.0, people_n);

		SDL_Surface* text_surface = TTF_RenderText_Blended_Wrapped(font, str, white, width / 3);
		SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
		SDL_Rect dst = {0, height, text_surface->w, text_surface->h};
		w = text_surface->w;

		SDL_RenderCopy(renderer, text_texture, NULL, &dst);
		SDL_FreeSurface(text_surface);
		SDL_DestroyTexture(text_texture);

		h = height;
		for (size_t j = 0; j < number_of_civs; j++) {
			sprintf(str, "Civ %zu population: %d", j + 1, civ_pops[j]);

			text_surface = TTF_RenderText_Blended(font, str, white);
			text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);

			dst = (SDL_Rect) {w, h, text_surface->w, text_surface->h};
			h += text_surface->h;

			SDL_RenderCopy(renderer, text_texture, NULL, &dst);
			SDL_FreeSurface(text_surface);
			SDL_DestroyTexture(text_texture);
		}

		sprintf(str, "FPS: %.lf", 1.0 / frame_time);
		text_surface = TTF_RenderText_Blended(font, str, white);
		text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
		dst = (SDL_Rect) {0, (height + 100) - text_surface->h, text_surface->w, text_surface->h};
		SDL_RenderCopy(renderer, text_texture, NULL, &dst);
		SDL_FreeSurface(text_surface);
		SDL_DestroyTexture(text_texture);

		SDL_RenderPresent(renderer);
		free(str);

		SDL_Event event;
		if (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				break;
			}
		}
	}

	printf("Closing...\n");

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	TTF_Quit();
	SDL_Quit();

	return 0;
}
