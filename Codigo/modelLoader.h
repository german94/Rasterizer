#ifndef DEF_MODELLOADER_H_
#define DEF_MODELLOADER_H_

#include "structHelper.h"
#include <SDL.h>
#include "sdlHelper.h"

bool LoadModel(char* path, Vec3DynamicArray* outVertices, Vec2DynamicArray* outUvs, Vec3DynamicArray* outNormals, UInt3DynamicArray* outFaces);
void RenderModel(Vec3DynamicArray* vertices, UInt3DynamicArray* faces, Vec2DynamicArray* uvs, Mat4 wvp, uint swidth, uint sheight, SDL_Surface* screenSurface);
void RenderFilledModel(Vec3DynamicArray* vertices, UInt3DynamicArray* faces, Vec2DynamicArray* uvs, Mat4 wvp, uint swidth, uint sheight, SDL_Surface* sf, float* depthBuffer);

#endif