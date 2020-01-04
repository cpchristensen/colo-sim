#include <stdio.h>
#include <stdlib.h>

#define SEED 10

typedef struct {
	int x, y;
} Point;

int main () {
	srand(SEED);

	size_t width = 640;
	size_t height = 480;
	size_t land_area = width * height * 0.3;
	int continents_n = 7;
	int x, y;

	Point* land_points = malloc(land_area * sizeof(Point));
	x = rand() % width;
	y = rand() % height;

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

	FILE* fp = fopen("maps/map.txt", "w");
	fprintf(fp, "%zu\n", land_area);

	for (size_t i = 0; i < land_area; i++) {
		fprintf(fp, "%d %d\n", land_points[i].x, land_points[i].y);
	}
	fclose(fp);
	free(land_points);
	printf("Wrote land points. to file.\n");

	return 0;
}
