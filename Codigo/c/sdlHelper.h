#ifndef _DEF_SDLH_H_
#define _DEF_SDLH_H_

#include <SDL.h>
#include <SDL2/SDL_ttf.h>        //sudo apt-get install libsdl2-ttf-dev, linkear con -lSDL2_ttf
#include "structHelper.h"
#include "mathHelper.h"

void putpixel(int x, int y, float z, float* depthBuffer, SDL_Surface* surface, Uint32 pixel);
Uint32 getpixel(SDL_Surface *surface, int x, int y);
void RenderText(SDL_Color clrFg, TTF_Font* font, char* buf, SDL_Surface* screenSurface);
void RenderTextR(SDL_Color clrFg, TTF_Font* font, char* buf, SDL_Surface* screenSurface, SDL_Rect* r);
float Max(float a, float b);
float Min(float a, float b);
float Clamp(float value);
float Interpolate2(float min, float max, float gradient);
float Interpolate1(float min, float max, float gradient);
float ComputeNDotL(Vec4 centerPoint, Vec4 vnFace, Vec3 lightPos); 
Uint32 Map(SDL_Surface* tex, float tu, float tv);
void Uint32ToVec4(Uint32 inColor, Vec3 outColor);
Uint32 Vec4ToUint32P(float _r, float _g, float _b);
void ProcessScanLine_m3(Vec3 color, Vertex* va, Vertex* vb, Vertex* vc, Vertex* vd, SDL_Surface* sf, float* depthBuffer, ScanLineData* data,  SDL_Surface* tex);
void DrawTriangle_m3(Vec3 color, Vertex* p1, Vertex* p2, Vertex* p3, SDL_Surface* sf, float* depthBuffer,  SDL_Surface* tex, Vec3 lightPos);
void ProcessScanLine_m2(Vec3 color, Vertex* va, Vertex* vb, Vertex* vc, Vertex* vd, SDL_Surface* sf, float* depthBuffer, ScanLineData* data,  SDL_Surface* tex);
void DrawTriangle_m2(Vec3 color, Vertex* p1, Vertex* p2, Vertex* p3, SDL_Surface* sf, float* depthBuffer,  SDL_Surface* tex);
void ProcessScanLine_m1(Vec3 va, Vec3 vb, Vec3 vc, Vec3 vd, SDL_Surface* sf, int Y);
void DrawTriangle_m1(Vec3 p1, Vec3 p2, Vec3 p3, SDL_Surface* sf);
void putpixel_m1(int x, int y, SDL_Surface* surface, Uint32 pixel);

#endif

