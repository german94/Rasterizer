#ifndef DEF_STRUCTH_H_
#define DEF_STRUCTH_H_

#include "mathHelper.h"

typedef struct
{
	Vec4 normal;
	Vec3 coordinates;
	Vec4 wcoordinates;
	Vec2 texCoordinates;
} Vertex;

typedef struct
{
	int currentY;
	float ndotla;
	float ndotlb;
	float ndotlc;
	float ndotld; 
} ScanLineData;

void CopyVec(float*, float*, int);

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


 /////////////////////////////////////////////////////////////////
 //                     Vec4 dynamic array                      //
 /////////////////////////////////////////////////////////////////

typedef struct {
   Vec4 *array;
   size_t used;
   size_t size;
 } Vec4DynamicArray;
 
 void initVec4DynamicArray(Vec4DynamicArray*, size_t);
 void insertVec4DynamicArray(Vec4DynamicArray*, Vec4);
 void freeVec4DynamicArray(Vec4DynamicArray*);

typedef struct
{
    Vec4DynamicArray* vertices;
    Vec2DynamicArray* uvs;
    Vec4DynamicArray* normals;
    UInt3DynamicArray* faces;

    float* color;

    float** wvp;
    float** world;

    int* max_dis;

} Model;

#endif