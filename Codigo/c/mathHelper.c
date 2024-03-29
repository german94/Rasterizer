#include "mathHelper.h"

void VecByScalar(float* in, float scalar, float* out, int d)
{
	int i;
	for(i = 0; i < d; i++)
		out[i] = in[i] * scalar;
}

void VecByVec(float* in1, float* in2, float* out, int d)
{
	int i;
	for(i = 0; i < d; i++)
		out[i] = in1[i] * in2[i];
}

float Dot3Prod(float* v1, Vec3 v2)//dot produc or scalar product
{
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

void Cross3Prod(Vec3 v1, Vec3 v2, Vec3 r)//producto determinante
{
	r[0] = v1[1] * v2[2] - v1[2] * v2[1];
	r[1] = v1[2] * v2[0] - v1[0] * v2[2];
	r[2] = v1[0] * v2[1] - v1[1] * v2[0];
}

void Sub3(float* v1, float* v2, Vec3 r)
{
	r[0] = v1[0] - v2[0];
	r[1] = v1[1] - v2[1];
	r[2] = v1[2] - v2[2];
}

void Normalize3(float* v)
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
			res[i][j] = m1[i][0] * m2[j][0] + m1[i][1] * m2[j][1] + m1[i][2] * m2[j][2] + m1[i][3] * m2[j][3];
	}
}

void Vec4Mat4Product(Vec4 v, Mat4 m, Vec4 r)
{
	r[0] = v[0] * m[0][0] + v[1] * m[0][1] + v[2] * m[0][2] + m[0][3];
	r[1] = v[0] * m[1][0] + v[1] * m[1][1] + v[2] * m[1][2] + m[1][3];
	r[2] = v[0] * m[2][0] + v[1] * m[2][1] + v[2] * m[2][2] + m[2][3];
	r[3] = v[0] * m[3][0] + v[1] * m[3][1] + v[2] * m[3][2] + m[3][3];
}

void CreateViewMatrix(Mat4 m, Vec3 cameraTarget, Vec3 cameraPos, Vec3 up)
{
	Vec3 w;   //eje z de la camara
	Sub3(cameraTarget, cameraPos, w);
	Normalize3(w);

	Vec3 u;   //eje x de la camara
	Cross3Prod(up, w, u);
	Normalize3(u);
	
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
//ejemplo de fieldOfView: PI / 4 (el angulo de vision la camara)
void CreateProjectionMatrix(Mat4 m, float nearPlane, float farPlane, float fieldOfView)
{
	m[0][0] = 1/tanf(fieldOfView*0.5);
	m[0][1] = m[0][2] = m[0][3] = 0;
	m[1][1] = 1/tanf(fieldOfView*0.5);
	m[1][0] = m[1][2] = m[1][3] = 0;
	m[2][2] = -farPlane / (farPlane - nearPlane);
	m[2][3] = -1;
	m[2][0] = m[2][1] = 0;
	m[3][2] = -nearPlane * farPlane / (farPlane - nearPlane);
	m[3][0] = m[3][1] = m[3][3] = 0;
}

void CreateRotationXMatrix(Mat4 m, float ang) 
{
	m[0][0] = 1;	m[0][1] = 0;	m[0][2] = 0;	m[0][3] = 0;
	m[1][0] = 0;	m[1][1] = cosf(ang);	m[1][2] = sinf(ang);	m[1][3] = 0;
	m[2][0] = 0;	m[2][1] = -sinf(ang);	m[2][2] = cosf(ang);	m[2][3] = 0;
	m[3][0] = 0;	m[3][1] = 0;	m[3][2] = 0;	m[3][3] = 1;
}

void CreateRotationYMatrix(Mat4 m, float ang)
{
	m[0][0] = cosf(ang);	m[0][1] = 0;	m[0][2] = -sinf(ang);	m[0][3] = 0;
	m[1][0] = 0;	m[1][1] = 1;	m[1][2] = 0;	m[1][3] = 0;
	m[2][0] = sinf(ang);	m[2][1] = 0;	m[2][2] = cosf(ang);	m[2][3] = 0;
	m[3][0] = 0;	m[3][1] = 0;	m[3][2] = 0;	m[3][3] = 1;
}

void CreateRotationZMatrix(Mat4 m, float ang)
{
	m[0][0] = cosf(ang);	m[0][1] = -sinf(ang);	m[0][2] = 0;	m[0][3] = 0;
	m[1][0] = sinf(ang);	m[1][1] = cosf(ang);	m[1][2] = 0;	m[1][3] = 0;
	m[2][0] = 0;	m[2][1] = 0;	m[2][2] = 1;	m[2][3] = 0;
	m[3][0] = 0;	m[3][1] = 0;	m[3][2] = 0;	m[3][3] = 1;
}

void CreateTranslationMatrix(Mat4 m, float x, float y, float z)
{
	m[0][0] = 1;	m[0][1] = 0;	m[0][2] = 0;	m[0][3] = x;
	m[1][0] = 0;	m[1][1] = 1;	m[1][2] = 0;	m[1][3] = y;
	m[2][0] = 0;	m[2][1] = 0;	m[2][2] = 1;	m[2][3] = z;
	m[3][0] = 0;	m[3][1] = 0;	m[3][2] = 0;	m[3][3] = 1;
}

void CreateScaleMatrix(Mat4 m, float sx, float sy, float sz)
{
	m[0][0] = sx;	m[0][1] = 0;	m[0][2] = 0;	m[0][3] = 0;
	m[1][0] = 0;	m[1][1] = sy;	m[1][2] = 0;	m[1][3] = 0;
	m[2][0] = 0;	m[2][1] = 0;	m[2][2] = sz;	m[2][3] = 0;
	m[3][0] = 0;	m[3][1] = 0;	m[3][2] = 0;	m[3][3] = 1;
}

void DisplayMatrix(Mat4 m)
{
	printf("%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n", m[0][0], m[0][1], m[0][2], m[0][3], 
		m[1][0], m[1][1], m[1][2], m[1][3], m[2][0], m[2][1], m[2][2], m[2][3], m[3][0], m[3][1], m[3][2], m[3][3]);
}

void Traspose(Mat4 m1)
{
	int i;
	for(i = 0; i < 4; i++)
	{
		int j;
		for(j = i; j < 4; j++)
		{
			float temp = m1[i][j];
			m1[i][j] = m1[j][i];
			m1[j][i] = temp;
		}
	}
}


void Traspose2(Mat4 m1, Mat4 res)
{
	int i;
	for(i = 0; i < 4; i++)
	{
		int j;
		for(j = 0; j < 4; j++)
		{
			res[j][i] = m1[i][j];
		}
	}
}

float ScalarProd(float *v1, float *v2)
{
	int i;
	float res = 0.0;
	for( i = 0; i < 4; i++)
	{
		res = res + v1[i]*v2[i];
	}
	return res;
}

void Mat4ProductTras(Mat4 m1, Mat4 m2, Mat4 res)
{
	int i, j;
	for(i = 0; i < 4; i++)
	{
		for(j = 0; j < 4; j++)
			res[j][i] = ScalarProd(&m1[i], &m2[j]);
	}
}

float Max(float a, float b)
{
	return (a > b) ? a : b;
}

float Min(float a, float b)
{
	return (a < b) ? a : b;
}


float Clamp(float value)
{ 
	return Max(0, Min(value, 1));
}

float Interpolate(float min, float max, float gradient, bool i1ON)
{
	if(!i1ON)
		return min + (max - min) * Clamp(gradient);

	float mini = (gradient < 1) ? gradient : 1;
    float maxi = (0 > mini) ? 0 : mini;
    return min + (max - min) *maxi;
}

float ComputeNDotL(Vec4 centerPoint, Vec4 vnFace, Vec3 lightPos) 
{
  Vec3 lightDirection;

  Sub3(lightPos, centerPoint, lightDirection);

  Normalize3(vnFace);
  
  Normalize3(lightDirection);

  return Max(0, Dot3Prod(vnFace, lightDirection));
}