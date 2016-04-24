#ifndef DEF_MATHH_H
#define DEF_MATHH_H

#include <stdio.h>
#include <math.h>

typedef float Vec4[4];
typedef float Vec3[3];
typedef float Vec2[2];
typedef int Pixel[2];
typedef unsigned int uint3[3];
typedef float Mat4[4][4];
typedef float Mat3[3][3];

void VecByScalar(float* in, float scalar, float* out, int d);
void VecByVec(float* in1, float* in2, float* out, int d);
float Dot3Prod(Vec3 v1, Vec3 v2);
void Cross3Prod(Vec3 v1, Vec3 v2, Vec3 r);
void Sub3(Vec3 v1, Vec3 v2, Vec3 r);
void Normalize3(Vec3 v);
void Identity(Mat4 m);
void Mat4Product(Mat4 m1, Mat4 m2, Mat4 res);
void Vec4Mat4Product(Vec4 v, Mat4 m, Vec4 r);
void CreateViewMatrix(Mat4 m, Vec3 cameraTarget, Vec3 cameraPos, Vec3 up);
void CreateProjectionMatrix(Mat4 m, float nearPlane, float farPlane, float fieldOfView);
void CreateRotationXMatrix(Mat4 m, float ang);
void CreateRotationYMatrix(Mat4 m, float ang);
void CreateRotationZMatrix(Mat4 m, float ang);
void CreateTranslationMatrix(Mat4 m, float x, float y, float z);
void CreateScaleMatrix(Mat4 m, float sx, float sy, float sz);
void DisplayMatrix(Mat4 m);
void Traspose(Mat4 m1);
void Traspose2(Mat4 m1, Mat4 res);
float ScalarProd(float *v1, float *v2);
void Mat4ProductTras(Mat4 m1, Mat4 m2, Mat4 res);

#endif