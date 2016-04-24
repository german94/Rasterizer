#ifndef DEF_MODELLOADER_H_
#define DEF_MODELLOADER_H_

#include "structHelper.h"
#include "sdlHelper.h"

bool LoadModel(char* path, Vec4DynamicArray *outVertices, Vec2DynamicArray *outUvs, Vec4DynamicArray *outNormals, UInt3DynamicArray* outFaces);
void RenderFilledModel_m3(Vec4DynamicArray *vertices, Vec2DynamicArray *uvs, Vec4DynamicArray *normals, UInt3DynamicArray* faces, Mat4 wvp, int swidth, int sheight, SDL_Surface* sf, float* depthBuffer, Mat4 world,  SDL_Surface* tex,  Vec3 lightPos);
void RenderFilledModel_m2(Vec4DynamicArray *vertices, Vec2DynamicArray *uvs, UInt3DynamicArray* faces, Mat4 wvp, int swidth, int sheight, SDL_Surface* sf, float* depthBuffer, SDL_Surface* tex);
void RenderFilledModel_m1(Vec4DynamicArray *vertices, UInt3DynamicArray* faces, Mat4 wvp, int swidth, int sheight, SDL_Surface* sf);

#endif