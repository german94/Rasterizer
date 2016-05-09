CC=gcc
CFLAGS = -g
SDLFLAGS = `sdl2-config --cflags --libs` -lm -lSDL2_ttf -w

all: renderizar

renderizar: test2.c mathHelper.c rasterHelper.c sdlHelper.c structHelper.c modelLoader.c
	$(CC) -o $@ $^ $(SDLFLAGS)

clean:
	rm -f *.o
	rm -f renderizar 

