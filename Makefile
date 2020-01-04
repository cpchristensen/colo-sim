all: main.c
	gcc main.c -lSDL2 -lSDL2_ttf -std=c99

mapgen: map-gen.c
	gcc map-gen.c -std=c99
