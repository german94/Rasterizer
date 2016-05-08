#ifndef DEF_MODELLOADER_H_
#define DEF_MODELLOADER_H_

#include "rasterHelper.h"

bool LoadModel(char* path, Model* data);
void RenderFilledModel_m3(Model* data, SDL_Surface* sf, float* depthBuffer, SDL_Surface* tex,  Vec3 lightPos);
void RenderFilledModel_m2(Model* data, SDL_Surface* sf, float* depthBuffer, SDL_Surface* tex);
void RenderFilledModel_m1(Model* data, SDL_Surface* sf);

#endif