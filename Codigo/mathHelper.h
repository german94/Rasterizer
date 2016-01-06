#ifndef DEF_MATHH_H
#define DEF_MATHH_H

typedef float[4] Vec4;
typedef float[3] Vec3;
typedef float[2] Vec2;
typedef float[4][4] Mat4;

float DotProd(Vec4 v1, Vec4 v2)
{
	float r = 0;

	for(int i = 0; i < 4; i++)
		r += v1[i] * v2[i];
}

void Product(Mat4* m1, Mat4* m2, Mat4* res)
{
	for(int i = 0; i < 4; i++)
	{
		for(int j = 0; j < 4; j++)
			res[i][j] = m1[i][0] * m2[0][j] + m1[i][1] * m2[1][j] + m1[i][2] * m2[2][j] + m1[i][3] * m2[3][j];
	}
}
#endif
