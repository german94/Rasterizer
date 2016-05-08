#ifndef _DEF_RAH_H_
#define _DEF_RAH_H_

#include "sdlHelper.h"

void ProcessScanLine_m3(Vec3 color, Vertex* va, Vertex* vb, Vertex* vc, Vertex* vd, SDL_Surface* sf, float* depthBuffer, ScanLineData* data,  SDL_Surface* tex);
void DrawTriangle_m3(Vec3 color, Vertex* p1, Vertex* p2, Vertex* p3, SDL_Surface* sf, float* depthBuffer,  SDL_Surface* tex, Vec3 lightPos);
void ProcessScanLine_m2(Vec3 color, Vertex* va, Vertex* vb, Vertex* vc, Vertex* vd, SDL_Surface* sf, float* depthBuffer, ScanLineData* data,  SDL_Surface* tex);
void DrawTriangle_m2(Vec3 color, Vertex* p1, Vertex* p2, Vertex* p3, SDL_Surface* sf, float* depthBuffer,  SDL_Surface* tex);
void ProcessScanLine_m1(Vec3 va, Vec3 vb, Vec3 vc, Vec3 vd, SDL_Surface* sf, int Y);
void DrawTriangle_m1(Vec3 p1, Vec3 p2, Vec3 p3, SDL_Surface* sf);

#endif