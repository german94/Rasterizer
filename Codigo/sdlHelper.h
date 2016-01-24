#ifndef _DEF_SDLH_H_
#define _DEF_SDLH_H_

#include <SDL.h>
#include <SDL2/SDL_ttf.h>        //sudo apt-get install libsdl2-ttf-dev, linkear con -lSDL2_ttf
#include "structHelper.h"
#include "mathHelper.h"

void putpixel(SDL_Surface *surface, int x, int y, float z, Uint32 pixel, int swidth, int sheight, float* depthBuffer);
Uint32 getpixel(SDL_Surface *surface, int x, int y);
void Translation(int (*Msrc)[3], int (*Mdst)[3], int cant_pixel, int *trans);
void RotationX(int (*Mdst)[3], int (*Msrc)[3], int angulo, int cant_pixel);
void Coordto2d(int (*Msrc)[3], int (*Mdst)[2], int cant_pixel);
void DrawBline( int x0, int x1, int y0, int y1, SDL_Surface* screenSurface);
void formar_cubo(int (*cubo)[3], int distancia);
void RenderText(SDL_Color, TTF_Font*, char* buf, SDL_Surface*);

#endif

