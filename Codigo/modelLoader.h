#ifndef DEF_MODELLOADER_H_
#define DEF_MODELLOADER_H_

#include "structHelper.h"
#include "sdlHelper.h"

//bool LoadModel(char* path, VertexDynamicArray*, UInt3DynamicArray*);
//void RenderFilledModel(VertexDynamicArray*, UInt3DynamicArray*, Mat4, int, int, SDL_Surface*, float*, Mat4, SDL_Surface* );


bool LoadModel(char* path, Vec4DynamicArray *outVertices, Vec2DynamicArray *outUvs, Vec3DynamicArray *outNormals, UInt3DynamicArray* outFaces);

void RenderFilledModel(Vec4DynamicArray *vertices, Vec2DynamicArray *uvs, Vec3DynamicArray *normals, UInt3DynamicArray* faces, Mat4 wvp, int swidth, int sheight, SDL_Surface* sf, float* depthBuffer, Mat4 world,  SDL_Surface* tex);

#endif