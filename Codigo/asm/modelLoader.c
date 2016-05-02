#include "modelLoader.h"
#include <float.h>

bool ESCALADO = false;
float factor;

bool LoadModel(char* path, Vec4DynamicArray *outVertices, Vec2DynamicArray *outUvs, Vec4DynamicArray *outNormals, UInt3DynamicArray* outFaces, Vec2_int max_dis)
{

	initVec4DynamicArray(outNormals, 0);
	
	initUInt3DynamicArray(outFaces, 1);
	
	initVec4DynamicArray(outVertices, 1);

	initVec2DynamicArray(outUvs, 0);

	FILE* file = fopen(path, "r");

	if(file == NULL)
	{
		printf("Error al cargar el archivo.\n");
		return false;
	}
	float maxy = 0.0; float maxx = 0.0;
	uint3 vertexIndex, normalIndex, uvIndex;
	int indice = 0;
	while(1)
	{
		//asumimos que la primera linea no va a tener mas de 128 caracteres
		char lineHeader[128];

		int res = fscanf(file, "%s", lineHeader);
		if(res == EOF)
			break;

		if(strcmp(lineHeader, "v") == 0)
		{
			Vec4 v;
			
			fscanf(file, "%f %f %f\n", &v[0], &v[1], &v[2]); v[3] = 1.0f;
			
			if(v[0] >= maxx)
			{
				maxx = v[0];
				max_dis[0] = indice; 
			}
			if(v[1] >= maxy)
			{	
				maxy = v[1];	
				max_dis[1] = indice;  
			}	

			insertVec4DynamicArray(outVertices, v);
			indice++;
		}
		else if(strcmp(lineHeader, "vt") == 0)
		{	
			if(outUvs->size ==0) 
				initVec2DynamicArray(outUvs, 1);

			Vec2 uv;
			fscanf(file, "%f %f\n", &uv[0], &uv[1]);
			uv[1] = 1 - uv[1];
			insertVec2DynamicArray(outUvs, uv);
		}
		else if(strcmp(lineHeader, "vn") == 0)
		{
			if(outNormals->size ==0) 
				initVec4DynamicArray(outNormals, 1);
			
			Vec4 normal;
			fscanf(file, "%f %f %f\n", &normal[0], &normal[1], &normal[2]); normal[3] = 1.0f;
			insertVec4DynamicArray(outNormals, normal);
		}
		else if(strcmp(lineHeader, "f") == 0)
		{	
			int matches, cero_a, cero_b, cero_c, uno_a, uno_b, uno_c, dos_a, dos_b, dos_c, tres_a, tres_b, tres_c; 
			
			if(outUvs->size != 0 && outNormals->size !=0)
			{
				matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d\n", &cero_a, &cero_b, &cero_c, &uno_a, &uno_b, &uno_c, &dos_a, &dos_b, &dos_c, &tres_a, &tres_b, &tres_c); 
				if(matches != 12 && matches != 9)
				{
					printf("Formato incompatible.\n");
					return false;
				}	
			}
			if(outUvs->size == 0 && outNormals->size !=0)
			{
				matches = fscanf(file, "%d//%d %d//%d %d//%d %d//%d\n", &cero_a, &cero_c, &uno_a, &uno_c, &dos_a, &dos_c, &tres_a, &tres_c);
				if(matches != 6 && matches!=8)
				{
					printf("Formato incompatible.\n");
					return false;
				}
			}
			if(outUvs->size != 0 && outNormals->size ==0)
			{
				matches = fscanf(file, "%d/%d %d/%d %d/%d %d/%d\n", &cero_a, &cero_b, &uno_a, &uno_b, &dos_a, &dos_b, &tres_a, &tres_b);
				if(matches != 6 && matches!=8)
				{
					printf("Formato incompatible.\n");
					return false;
				}
			}
			if(outUvs->size == 0 && outNormals->size == 0)
			{
				matches = fscanf(file, "%d %d %d %d\n", &cero_a, &uno_a, &dos_a, &tres_a);
				if(matches != 3 && matches!=4)
				{
					printf("Formato incompatible.\n");
					return false;
				}
			}

			cero_b--; uno_b--; dos_b--; tres_b--;
			cero_a--; cero_c--;
			uno_a--; uno_c--; 
			dos_a--; dos_c--;
			tres_a--; tres_c--;

			vertexIndex[0] = cero_a ; vertexIndex[1] = uno_a; vertexIndex[2] = dos_a;
			uvIndex[0] =  cero_b; uvIndex[1] = uno_b; uvIndex[2] = dos_b;	
			normalIndex[0] =  cero_c; normalIndex[1] = uno_c; normalIndex[2] = dos_c;
			insertUInt3DynamicArray(outFaces, vertexIndex);
			insertUInt3DynamicArray(outFaces, uvIndex);
			insertUInt3DynamicArray(outFaces, normalIndex);

			if(matches== 8 || matches == 12 || matches == 4)
			{
				vertexIndex[0] = cero_a ; vertexIndex[1] = dos_a; vertexIndex[2] = tres_a;
				uvIndex[0] =  cero_b; uvIndex[1] = dos_b; uvIndex[2] = tres_b;	
				normalIndex[0] =  cero_c; normalIndex[1] = dos_c; normalIndex[2] = tres_c;
				insertUInt3DynamicArray(outFaces, vertexIndex);
				insertUInt3DynamicArray(outFaces, uvIndex);
				insertUInt3DynamicArray(outFaces, normalIndex);
			}

		}
	}
}
    

void escalando(Vec4DynamicArray *vertices, Mat4 wvp, int swidth, int sheight, Vec2_int max_dis)
{
	Vec4 aux = {1, 0, 0, 0};
	Vec4 maxx, maxy, resx, resy;
	float escalado_x, escalado_y;
	CopyVec(maxx, vertices->array[max_dis[0]], 4);
	CopyVec(maxy, vertices->array[max_dis[1]], 4);

	Vec4Mat4ProductASM(maxx, wvp, resx, aux);
	float x_final = (resx[0]/resx[3])*swidth + swidth*0.5 ;
	if (x_final < swidth*0.7)
		escalado_x = 1.0;
	else
		escalado_x = (swidth*0.7)/x_final;
	
	if(x_final < swidth*0.6)
		escalado_x = 1.7;
	
	Vec4Mat4ProductASM(maxy, wvp, resy, aux);
	float y_final = -(resy[1]/resy[3])*sheight + sheight*0.7 ;
	if (y_final > 0 )
		escalado_y = 1.0;
	else
		escalado_y = -(sheight*0.35)/y_final;

	if(y_final > sheight*0.3)
		escalado_y = 1.7;

	if(escalado_x > 1.0 && escalado_y > 1.0)
		factor = 1.7;
	else
		factor = (escalado_y < escalado_x) ? escalado_y : escalado_x;

	ESCALADO = true;  
}

void RenderFilledModel_m3(Vec3 color, Vec4DynamicArray *vertices, Vec2DynamicArray *uvs, Vec4DynamicArray *normals, UInt3DynamicArray* faces, Mat4 wvp, int swidth, int sheight, SDL_Surface* sf, float* depthBuffer, Mat4 world,  SDL_Surface* tex, Vec3 lightPos, Vec2_int max_dis)
{	
	Vec4 aux = {1, 0, 0, 0};
	int  j;

	if (!ESCALADO)
    	escalando(vertices, wvp, swidth, sheight, max_dis);
	
	for(j = 0; j < faces->used; j= j+3)//NOTAR QUE COMO GUARDA PARA CADA TRIANGULO LE CORRESPONDEN 3 LUGARES
    {   	   
	    Vec4 res1, res2, res3, res1n, res2n, res3n, res1w, res2w, res3w;
	    Vec3 v1, v2, v3, v1n, v2n, v3n, v1w, v2w, v3w;

	    Vec4 vertexA, vertexB, vertexC;
		CopyVec(vertexA, vertices->array[faces->array[j][0]], 4);
		CopyVec(vertexB, vertices->array[faces->array[j][1]], 4);
		CopyVec(vertexC, vertices->array[faces->array[j][2]], 4);
		VecByScalar(vertexA, factor, vertexA, 3);
        VecByScalar(vertexB, factor, vertexB, 3);
        VecByScalar(vertexC, factor, vertexC, 3);


	    Vec4Mat4ProductASM(vertexA, wvp, res1, aux);
	    v1[0] = (res1[0]/res1[3])*swidth + swidth*0.5 ;
	    v1[1] = (-res1[1]/res1[3])*sheight+ sheight*0.7; 
	    v1[2] = res1[2] / res1[3];

	    Vec4Mat4ProductASM(vertexB, wvp, res2, aux);
	    v2[0] = (res2[0]/res2[3])*swidth + swidth*0.5 ;
	    v2[1] = (-res2[1]/res2[3])*sheight + sheight*0.7; 
	    v2[2] = res2[2] / res2[3];

	    Vec4Mat4ProductASM(vertexC, wvp, res3, aux);
	    v3[0] = (res3[0]/res3[3])*swidth + swidth*0.5 ;
	    v3[1] = (-res3[1]/res3[3])*sheight + sheight*0.7; 
	 	v3[2] = res3[2] / res3[3];

	 	///////////////////////////
 		Vec4Mat4ProductASM(vertexA, world, res1w, aux);
 		v1w[0] = res1w[0]; v1w[1] = res1w[1]; v1w[2] = res1w[2];

        Vec4Mat4ProductASM(vertexB, world, res2w, aux);
		v2w[0] = res2w[0]; v2w[1] = res2w[1]; v2w[2] = res2w[2];

        Vec4Mat4ProductASM(vertexC, world, res3w, aux);
        v3w[0] = res3w[0]; v3w[1] = res3w[1]; v3w[2] = res3w[2];        ////////////

        ////////////////////////////

        Vec4 vertexAn, vertexBn, vertexCn;
		CopyVec(vertexAn, normals->array[faces->array[j+2][0]], 4);
		CopyVec(vertexBn, normals->array[faces->array[j+2][1]], 4);
		CopyVec(vertexCn, normals->array[faces->array[j+2][2]], 4);


        Vec4Mat4ProductASM(vertexAn, world, res1n, aux);
        v1n[0] = res1n[0]; v1n[1] = res1n[1]; v1n[2] = res1n[2];        ////////////

        Vec4Mat4ProductASM(vertexBn, world, res2n, aux);
		v2n[0] = res2n[0]; v2n[1] = res2n[1]; v2n[2] = res2n[2]; 

        Vec4Mat4ProductASM(vertexCn, world, res3n, aux);
		v3n[0] = res3n[0]; v3n[1] = res3n[1]; v3n[2] = res3n[2];  

		Vec2 v1t, v2t, v3t;
		if(tex != NULL)
		{
			CopyVec(v1t, uvs->array[faces->array[j+1][0]], 2);
			CopyVec(v2t, uvs->array[faces->array[j+1][1]], 2);
			CopyVec(v3t, uvs->array[faces->array[j+1][2]], 2);
		}
		
		DrawTriangle_m3(color, v1, v2, v3, v1n, v2n, v3n, v1w, v2w, v3w, swidth, sheight, sf, depthBuffer, tex, v1t, v2t, v3t, lightPos);
	}
}



void RenderFilledModel_m2(Vec3 color, Vec4DynamicArray *vertices, Vec2DynamicArray *uvs, UInt3DynamicArray* faces, Mat4 wvp, int swidth, int sheight, SDL_Surface* sf, float* depthBuffer, SDL_Surface* tex, Vec2_int max_dis)
{	
	Vec4 aux = {1, 0, 0, 0};
	int  j;

	if (!ESCALADO)
    	escalando(vertices, wvp, swidth, sheight, max_dis);	
	
	for(j = 0; j < faces->used; j= j+3)//NOTAR QUE COMO GUARDA PARA CADA TRIANGULO LE CORRESPONDEN 3 LUGARES
    {   	   
	    Vec4 res1, res2, res3;
	    Vec3 v1, v2, v3;

	    Vec4 vertexA, vertexB, vertexC;
		CopyVec(vertexA, vertices->array[faces->array[j][0]], 4);
		CopyVec(vertexB, vertices->array[faces->array[j][1]], 4);
		CopyVec(vertexC, vertices->array[faces->array[j][2]], 4);
		VecByScalar(vertexA, factor, vertexA, 3);
        VecByScalar(vertexB, factor, vertexB, 3);
        VecByScalar(vertexC, factor, vertexC, 3);


	    Vec4Mat4ProductASM(vertexA, wvp, res1, aux);
	    v1[0] = (res1[0]/res1[3])*swidth + swidth*0.5 ;
	    v1[1] = (-res1[1]/res1[3])*sheight+ sheight*0.7; 
	    v1[2] = res1[2] / res1[3];

	    Vec4Mat4ProductASM(vertexB, wvp, res2, aux);
	    v2[0] = (res2[0]/res2[3])*swidth + swidth*0.5 ;
	    v2[1] = (-res2[1]/res2[3])*sheight + sheight*0.7; 
	    v2[2] = res2[2] / res2[3];

	    Vec4Mat4ProductASM(vertexC, wvp, res3, aux);
	    v3[0] = (res3[0]/res3[3])*swidth + swidth*0.5 ;
	    v3[1] = (-res3[1]/res3[3])*sheight + sheight*0.7; 
	 	v3[2] = res3[2] / res3[3];

	 	///////////////////////////
		Vec2 v1t, v2t, v3t;
		if(tex != NULL)
		{
			float a;
			CopyVec(v1t, uvs->array[faces->array[j+1][0]], 2);
			CopyVec(v2t, uvs->array[faces->array[j+1][1]], 2);
			CopyVec(v3t, uvs->array[faces->array[j+1][2]], 2);
		}
		
		DrawTriangle_m2(color, v1, v2, v3, swidth, sheight, sf, depthBuffer, tex, v1t, v2t, v3t);
	}
}




void RenderFilledModel_m1(Vec4DynamicArray *vertices, UInt3DynamicArray* faces, Mat4 wvp, int swidth, int sheight, SDL_Surface* sf, Vec2_int max_dis)
{	
	int  j;
	Vec4 aux = {1, 0, 0, 0};
	
	if (!ESCALADO)
    	escalando(vertices, wvp, swidth, sheight, max_dis);

	for(j = 0; j < faces->used; j= j+3)//NOTAR QUE COMO GUARDA PARA CADA TRIANGULO LE CORRESPONDEN 3 LUGARES
    {   
    	Vec4 res1, res2, res3;
    	
	    Vec3 v1, v2, v3;

	    Vec4 vertexA, vertexB, vertexC;
		CopyVec(vertexA, vertices->array[faces->array[j][0]], 4);
		CopyVec(vertexB, vertices->array[faces->array[j][1]], 4);
		CopyVec(vertexC, vertices->array[faces->array[j][2]], 4);
        VecByScalar(vertexA, factor, vertexA, 3);
        VecByScalar(vertexB, factor, vertexB, 3);
        VecByScalar(vertexC, factor, vertexC, 3);

	    Vec4Mat4ProductASM(vertexA, wvp, res1, aux);
	    v1[0] = (res1[0]/res1[3])*swidth + swidth*0.5 ;
	    v1[1] = (-res1[1]/res1[3])*sheight+ sheight*0.7; 
	    v1[2] = res1[2] / res1[3];

	    Vec4Mat4ProductASM(vertexB, wvp, res2, aux);
	    v2[0] = (res2[0]/res2[3])*swidth + swidth*0.5 ;
	    v2[1] = (-res2[1]/res2[3])*sheight + sheight*0.7; 
	    v2[2] = res2[2] / res2[3];

	    Vec4Mat4ProductASM(vertexC, wvp, res3, aux);
	    v3[0] = (res3[0]/res3[3])*swidth + swidth*0.5 ;
	    v3[1] = (-res3[1]/res3[3])*sheight + sheight*0.7; 
	 	v3[2] = res3[2] / res3[3];

		DrawTriangle_m1(v1, v2, v3, swidth, sheight, sf);
	}
}

