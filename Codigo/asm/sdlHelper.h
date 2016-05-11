#ifndef _DEF_SDLH_H_
#define _DEF_SDLH_H_

#include <SDL.h>
#include <SDL2/SDL_ttf.h> 
#include "structHelper.h"    
#include "mathHelper.h"

void putpixel(int x, int y, float z, float* depthBuffer, SDL_Surface* surface, Uint32 pixel);
Uint32 getpixel(SDL_Surface *surface, int x, int y);
void RenderText(SDL_Color clrFg, TTF_Font* font, char* buf, SDL_Surface* screenSurface);
void RenderTextR(SDL_Color clrFg, TTF_Font* font, char* buf, SDL_Surface* screenSurface, SDL_Rect* r);
Uint32 Map(SDL_Surface* tex, float tu, float tv);
void Uint32ToVec4(Uint32 inColor, Vec3 outColor);
Uint32 Vec4ToUint32P(float _r, float _g, float _b);
void putpixel_m1(int x, int y, SDL_Surface* surface, Uint32 pixel);

#endif

