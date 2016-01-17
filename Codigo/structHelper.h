#ifndef DEF_STRUCTH_H_
#define DEF_STRUCTH_H_

#define true 1
#define false 0
#include "mathHelper.h"

typedef int bool;
//typedef unsigned int uint;

/////////////////////////////////////////////////////////////////
//                     uint3 dynamic array                     //
/////////////////////////////////////////////////////////////////
typedef struct {
  uint3 *array;
  size_t used;
  size_t size;
} UInt3DynamicArray;
    
void initUInt3DynamicArray(UInt3DynamicArray*, size_t);
void insertUInt3DynamicArray(UInt3DynamicArray*, uint3);
void freeUint3DynamicArray(UInt3DynamicArray*);

/////////////////////////////////////////////////////////////////
//                     Vec3 dynamic array                      //
/////////////////////////////////////////////////////////////////
typedef struct {
  Vec3 *array;
  size_t used;
  size_t size;
} Vec3DynamicArray;

void initVec3DynamicArray(Vec3DynamicArray*, size_t);
void insertVec3DynamicArray(Vec3DynamicArray*, Vec3);
void freeVec3DynamicArray(Vec3DynamicArray*);


/////////////////////////////////////////////////////////////////
//                     Vec2 dynamic array                      //
/////////////////////////////////////////////////////////////////
typedef struct {
  Vec2 *array;
  size_t used;
  size_t size;
} Vec2DynamicArray;

void initVec2DynamicArray(Vec2DynamicArray*, size_t);
void insertVec2DynamicArray(Vec2DynamicArray*, Vec2);
void freeVec2DynamicArray(Vec2DynamicArray*);

#endif