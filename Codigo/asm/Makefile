CC=gcc
CFLAGS = -g
NASM=nasm
NASMFLAGS=-f elf64 -g
SDLFLAGS = `sdl2-config --cflags --libs` -lm -lSDL2_ttf -w

all: asm.o renderizar

renderizar: test2.c mathHelper.c rasterHelper.c sdlHelper.c structHelper.c modelLoader.c
	$(CC) -o $@ $^ $(SDLFLAGS) asm.o

asm.o: mathHelper.asm
	$(NASM) $(NASMFLAGS) $< -o $@

clean:
	rm -f *.o
	rm -f renderizar 

