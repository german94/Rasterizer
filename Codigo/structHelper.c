#include "structHelper.h"

/////////////////////////////////////////////////////////////////
//                     uint3 dynamic array                     //
/////////////////////////////////////////////////////////////////
void initUInt3DynamicArray(UInt3DynamicArray *a, size_t initialSize) 
{
  a->array = (uint3 *)malloc(initialSize * sizeof(uint3));
  a->used = 0;
  a->size = initialSize;
}

void insertUInt3DynamicArray(UInt3DynamicArray *a, uint3 element) 
{
  if (a->used == a->size) {
    a->size *= 2;
    a->array = (uint3 *)realloc(a->array, a->size * sizeof(uint3));
  }
  a->array[a->used][0] = element[0];
  a->array[a->used][1] = element[1];
  a->array[a->used][2] = element[2];

  a->used++;
}

void freeUint3DynamicArray(UInt3DynamicArray *a) 
{
  free(a->array);
  a->array = NULL;
  a->used = a->size = 0;
}

/////////////////////////////////////////////////////////////////
//                     Vec3 dynamic array                      //
/////////////////////////////////////////////////////////////////
void initVec3DynamicArray(Vec3DynamicArray *a, size_t initialSize) 
{
  a->array = (Vec3 *)malloc(initialSize * sizeof(Vec3));
  a->used = 0;
  a->size = initialSize;
}

void insertVec3DynamicArray(Vec3DynamicArray *a, Vec3 element) 
{
  if (a->used == a->size) {
    a->size *= 2;
    a->array = (Vec3 *)realloc(a->array, a->size * sizeof(Vec3));
  }
  a->array[a->used][0] = element[0];
  a->array[a->used][1] = element[1];
  a->array[a->used][2] = element[2];

  a->used++;
}

void freeVec3DynamicArray(Vec3DynamicArray *a) 
{
  free(a->array);
  a->array = NULL;
  a->used = a->size = 0;
}

/////////////////////////////////////////////////////////////////
//                     Vec2 dynamic array                      //
/////////////////////////////////////////////////////////////////
void initVec2DynamicArray(Vec2DynamicArray *a, size_t initialSize) 
{
  a->array = (Vec2 *)malloc(initialSize * sizeof(Vec2));
  a->used = 0;
  a->size = initialSize;
}

void insertVec2DynamicArray(Vec2DynamicArray *a, Vec2 element) 
{
  if (a->used == a->size) {
    a->size *= 2;
    a->array = (Vec2 *)realloc(a->array, a->size * sizeof(Vec2));
  }
  a->array[a->used][0] = element[0];
  a->array[a->used][1] = element[1];
  a->array[a->used][2] = element[2];

  a->used++;
}

void freeVec2DynamicArray(Vec2DynamicArray *a) 
{
  free(a->array);
  a->array = NULL;
  a->used = a->size = 0;
}