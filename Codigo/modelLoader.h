#ifndef DEF_MODELLOADER_H_
#define DEF_MODELLOADER_H_

#include "structHelper.h"
#include "sdlHelper.h"

bool LoadModel(char* path, VertexDynamicArray*, UInt3DynamicArray*);
void RenderFilledModel(VertexDynamicArray*, UInt3DynamicArray*, Mat4, int, int, SDL_Surface*, float*, Mat4);

#endif