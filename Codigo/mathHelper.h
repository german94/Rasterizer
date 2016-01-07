#ifndef DEF_MATHH_H
#define DEF_MATHH_H

#include <math.h>

typedef float Vec4[4];
typedef float Vec3[3];
typedef float Vec2[2];
typedef float Mat4[4][4];

float Dot3Prod(Vec4 v1, Vec4 v2)
{
	float r = 0;
	int i;

	for(i = 0; i < 4; i++)
		r += v1[i] * v2[i];

	return r;
}

void Cross3Prod(Vec4 v1, Vec4 v2, Vec4 r)
{
	r[0] = v1[1] * v2[2] - v1[2] * v2[1];
	r[1] = v1[2] * v2[0] - v1[0] * v2[2];
	r[2] = v1[0] * v2[1] - v1[1] * v2[0];
}

void Sub3(Vec3 v1, Vec3 v2, Vec3 r)
{
	r[0] = v1[0] - v2[0];
	r[1] = v1[1] - v2[1];
	r[2] = v1[2] - v2[2];
}

void Normalize3(Vec3 v)
{
	float n = sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	v[0] /= n;
	v[1] /= n;
	v[2] /= n;
}

void Identity(Mat4 m)
{
	m[0][0] = m[1][1] = m[2][2] = m[3][3] = 1;
	m[0][1] = m[0][2] = m[0][3] = 0;
	m[1][0] = m[1][2] = m[1][3] = 0;	
	m[2][0] = m[2][1] = m[2][3] = 0;
	m[3][0] = m[3][1] = m[3][2] = 0;
}

void Mat4Product(Mat4 m1, Mat4 m2, Mat4 res)
{
	int i, j;
	for(i = 0; i < 4; i++)
	{
		for(j = 0; j < 4; j++)
			res[i][j] = m1[i][0] * m2[0][j] + m1[i][1] * m2[1][j] + m1[i][2] * m2[2][j] + m1[i][3] * m2[3][j];
	}
}

//vector fila
void Vec4Mat4Product(Vec4 v, Mat4 m, Vec4 r)
{
	int i;
	int j;
	r[0] = v[0] * m[0][0] + v[1] * m[1][0] + v[2] * m[2][0] + v[3] * m[3][0];
	r[1] = v[0] * m[0][1] + v[1] * m[1][1] + v[2] * m[2][1] + v[3] * m[3][1];
	r[2] = v[0] * m[0][2] + v[1] * m[1][2] + v[2] * m[2][2] + v[3] * m[3][2];
	r[3] = v[0] * m[0][3] + v[1] * m[1][3] + v[2] * m[2][3] + v[3] * m[3][3];
}

void CreateViewMatrix(Mat4 m, Vec3 cameraTarget, Vec3 cameraPos, Vec3 up)
{
	Vec3 w;   //eje z de la camara
	Sub3(cameraTarget, cameraPos, w);
	//Normalize3(w);

	Vec3 u;   //eje x de la camara
	Cross3Prod(up, u, w);
	//Normalize3(u);

	Vec3 v;   //eje y de la camara
	Cross3Prod(w, u, v);

	m[0][0] = u[0];		m[0][1] = v[0];		m[0][2] = w[0];		m[0][3] = 0;
	m[1][0] = u[1];		m[1][1] = v[1];		m[1][2] = w[1];		m[1][3] = 0;
	m[2][0] = u[2];		m[2][1] = v[2];		m[2][2] = w[2];		m[2][3] = 0;

	m[3][0] = -Dot3Prod(cameraPos, u);
	m[3][1] = -Dot3Prod(cameraPos, v);
	m[3][2] = -Dot3Prod(cameraPos, w);
	m[3][3] = 1;
}

//ejemplo aspectRatio: 800/600
//ejemplo de fieldOfView: PI / 4 (el angulo de vision la camara)
void CreateProjectionMatrix(Mat4 m, float nearPlane, float farPlane, float fieldOfView, float aspectRatio)
{
	m[0][0] = 1/(aspectRatio * tanf(fieldOfView / 2));
	m[0][1] = m[0][2] = m[0][3] = 0;
	m[1][1] = 1/tanf(fieldOfView / 2);
	m[1][0] = m[1][2] = m[1][3] = 0;
	m[2][2] = farPlane / (farPlane - nearPlane);
	m[2][3] = 1;
	m[2][0] = m[2][1] = 0;
	m[3][2] = -nearPlane * farPlane / (farPlane - nearPlane);
	m[3][0] = m[3][1] = m[3][3] = 0;
}

void DisplayMatrix(Mat4 m)
{
	printf("%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n", m[0][0], m[0][1], m[0][2], m[0][3], 
		m[1][0], m[1][1], m[1][2], m[1][3], m[2][0], m[2][1], m[2][2], m[2][3], m[3][0], m[3][1], m[3][2], m[3][3]);
}

#endif