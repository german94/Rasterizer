#ifndef _DEF_SDLH_H_
#define _DEF_SDLH_H_

#include <SDL.h>
#include "structHelper.h"

void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel);
Uint32 getpixel(SDL_Surface *surface, int x, int y);
void Translation(int (*Msrc)[3], int (*Mdst)[3], int cant_pixel, int *trans);
void RotationX(int (*Mdst)[3], int (*Msrc)[3], int angulo, int cant_pixel);
void Coordto2d(int (*Msrc)[3], int (*Mdst)[2], int cant_pixel);
void DrawBline( int x0, int x1, int y0, int y1, SDL_Surface* screenSurface);
void formar_cubo(int (*cubo)[3], int distancia);
#endif

