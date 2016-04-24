#include "structHelper.h"


void CopyVec(float* v1, float* v2, int dimension)
{
    int i;
    for(i = 0; i < dimension; i++)
        v1[i] = v2[i];
}


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
 //                     Vec4 dynamic array                      //
 /////////////////////////////////////////////////////////////////
 void initVec4DynamicArray(Vec4DynamicArray *a, size_t initialSize) 
 {
   a->array = (Vec4 *)malloc(initialSize * sizeof(Vec4));
   a->used = 0;
   a->size = initialSize;
 }
 
 void insertVec4DynamicArray(Vec4DynamicArray *a, Vec4 element) 
 {
   if (a->used == a->size) {
     a->size *= 2;
     a->array = (Vec4 *)realloc(a->array, a->size * sizeof(Vec4));
   }
   a->array[a->used][0] = element[0];
   a->array[a->used][1] = element[1];
   a->array[a->used][2] = element[2];
   a->array[a->used][3] = element[3];
 
   a->used++;
 }
 
 void freeVec4DynamicArray(Vec4DynamicArray *a) 
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