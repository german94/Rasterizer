#include "modelLoader.h"
#include <float.h>


bool LoadModel(char* path, Vec4DynamicArray *outVertices, Vec2DynamicArray *outUvs, Vec4DynamicArray *outNormals, UInt3DynamicArray* outFaces)
{

	initVec4DynamicArray(outNormals, 1);
	
	initUInt3DynamicArray(outFaces, 1);
	
	initVec4DynamicArray(outVertices, 1);

	initVec2DynamicArray(outUvs, 0);

	FILE* file = fopen(path, "r");

	if(file == NULL)
	{
		printf("Error al cargar el archivo.\n");
		return false;
	}

	uint3 vertexIndex, normalIndex, uvIndex;
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
			
			insertVec4DynamicArray(outVertices, v);
		}
		else if(strcmp(lineHeader, "vt") == 0)
		{	
			if(outUvs->size ==0) 
				initVec2DynamicArray(outUvs, 1);

			Vec2 uv;
			fscanf(file, "%f %f\n", &uv[0], &uv[1]);
			insertVec2DynamicArray(outUvs, uv);
		}
		else if(strcmp(lineHeader, "vn") == 0)
		{
			Vec4 normal;
			fscanf(file, "%f %f %f\n", &normal[0], &normal[1], &normal[2]); normal[3] = 1.0f;
			insertVec4DynamicArray(outNormals, normal);
		}
		else if(strcmp(lineHeader, "f") == 0)
		{	
			if(outUvs->size != 0 )
			{
				int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], 
				&vertexIndex[1], &uvIndex[1], &normalIndex[1], 
				&vertexIndex[2], &uvIndex[2], &normalIndex[2]);

				if(matches != 9)
				{
					printf("Formato incompatible.\n");
					return false;
				}

				uvIndex[0]--;
				uvIndex[1]--;
				uvIndex[2]--;
			}
			else
			{
				int matches = fscanf(file, "%d//%d %d//%d %d//%d\n", &vertexIndex[0], &normalIndex[0], 
				&vertexIndex[1], &normalIndex[1], 
				&vertexIndex[2], &normalIndex[2]);

				if(matches != 6)
				{
					printf("Formato incompatible.\n");
					return false;
				}
			}

			vertexIndex[0]--;
			vertexIndex[1]--;
			vertexIndex[2]--;
			normalIndex[0]--;
			normalIndex[1]--;
			normalIndex[2]--;

			insertUInt3DynamicArray(outFaces, vertexIndex);
			insertUInt3DynamicArray(outFaces, uvIndex);
			insertUInt3DynamicArray(outFaces, normalIndex);
		}
	}

}
    
void RenderFilledModel(Vec4DynamicArray *vertices, Vec2DynamicArray *uvs, Vec4DynamicArray *normals, UInt3DynamicArray* faces, Mat4 wvp, int swidth, int sheight, SDL_Surface* sf, float* depthBuffer, Mat4 world,  SDL_Surface* tex, Vec3 lightPos)
{	
	Vec4 aux = {1, 0, 0, 0};
	int  j;
	
	for(j = 0; j < faces->used; j= j+3)//NOTAR QUE COMO GUARDA PARA CADA TRIANGULO LE CORRESPONDEN 3 LUGARES
    {   	   
	    Vec4 res1, res2, res3, res1n, res2n, res3n, res1w, res2w, res3w;
	    Vec3 v1, v2, v3, v1n, v2n, v3n, v1w, v2w, v3w;

	    Vec4 vertexA, vertexB, vertexC;
		CopyVec4(vertexA, vertices->array[faces->array[j][0]], 4);
		CopyVec4(vertexB, vertices->array[faces->array[j][1]], 4);
		CopyVec4(vertexC, vertices->array[faces->array[j][2]], 4);


	    Vec4Mat4ProductASM(vertexA, wvp, res1, aux);
	    v1[0] = (res1[0]/res1[3])*swidth/**0.5*/ + swidth*0.5 ;
	    v1[1] = (res1[1]/res1[3])*sheight/**0.5*/+ sheight*0.5; 
	    v1[2] = res1[2] / res1[3];

	    Vec4Mat4ProductASM(vertexB, wvp, res2, aux);
	    v2[0] = (res2[0]/res2[3])*swidth/**0.5*/ + swidth*0.5 ;
	    v2[1] = (res2[1]/res2[3])*sheight/**0.5*/ + sheight*0.5; 
	    v2[2] = res2[2] / res2[3];

	    Vec4Mat4ProductASM(vertexC, wvp, res3, aux);
	    v3[0] = (res3[0]/res3[3])*swidth/**0.5*/ + swidth*0.5 ;
	    v3[1] = (res3[1]/res3[3])*sheight/**0.5*/ + sheight*0.5; 
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
		CopyVec4(vertexAn, normals->array[faces->array[j+2][0]], 4);
		CopyVec4(vertexBn, normals->array[faces->array[j+2][1]], 4);
		CopyVec4(vertexCn, normals->array[faces->array[j+2][2]], 4);


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
		Vec3 color;
		//color[0] = 0.25f + (j % faces->used) * 0.75f / faces->used;
		color[0] = 1.0f;
		color[1] = color[0];
		color[2] = color[0];

		DrawTriangle(v1, v2, v3, v1n, v2n, v3n, v1w, v2w, v3w, color, swidth, sheight, sf, depthBuffer, tex, v1t, v2t, v3t, lightPos);
	}
}



void RenderFilledModel_tex(Vec4DynamicArray *vertices, Vec2DynamicArray *uvs, UInt3DynamicArray* faces, Mat4 wvp, int swidth, int sheight, SDL_Surface* sf, float* depthBuffer, SDL_Surface* tex)
{	
	Vec4 aux = {1, 0, 0, 0};
	int  j;
	
	for(j = 0; j < faces->used; j= j+3)//NOTAR QUE COMO GUARDA PARA CADA TRIANGULO LE CORRESPONDEN 3 LUGARES
    {   	   
	    Vec4 res1, res2, res3;
	    Vec3 v1, v2, v3;

	    Vec4 vertexA, vertexB, vertexC;
		CopyVec4(vertexA, vertices->array[faces->array[j][0]], 4);
		CopyVec4(vertexB, vertices->array[faces->array[j][1]], 4);
		CopyVec4(vertexC, vertices->array[faces->array[j][2]], 4);


	    Vec4Mat4ProductASM(vertexA, wvp, res1, aux);
	    v1[0] = (res1[0]/res1[3])*swidth/**0.5*/ + swidth*0.5 ;
	    v1[1] = (res1[1]/res1[3])*sheight/**0.5*/+ sheight*0.5; 
	    v1[2] = res1[2] / res1[3];

	    Vec4Mat4ProductASM(vertexB, wvp, res2, aux);
	    v2[0] = (res2[0]/res2[3])*swidth/**0.5*/ + swidth*0.5 ;
	    v2[1] = (res2[1]/res2[3])*sheight/**0.5*/ + sheight*0.5; 
	    v2[2] = res2[2] / res2[3];

	    Vec4Mat4ProductASM(vertexC, wvp, res3, aux);
	    v3[0] = (res3[0]/res3[3])*swidth/**0.5*/ + swidth*0.5 ;
	    v3[1] = (res3[1]/res3[3])*sheight/**0.5*/ + sheight*0.5; 
	 	v3[2] = res3[2] / res3[3];

	 	///////////////////////////
		Vec2 v1t, v2t, v3t;
		if(tex != NULL)
		{
			CopyVec(v1t, uvs->array[faces->array[j+1][0]], 2);
			CopyVec(v2t, uvs->array[faces->array[j+1][1]], 2);
			CopyVec(v3t, uvs->array[faces->array[j+1][2]], 2);
		}
		Vec3 color;
		//color[0] = 0.25f + (j % faces->used) * 0.75f / faces->used;
		color[0] = 1.0f;
		color[1] = color[0];
		color[2] = color[0];

		DrawTriangle_tex(v1, v2, v3, color, swidth, sheight, sf, depthBuffer, tex, v1t, v2t, v3t);
	}
}




void RenderFilledModel_esq(Vec4DynamicArray *vertices, UInt3DynamicArray* faces, Mat4 wvp, int swidth, int sheight, SDL_Surface* sf)
{	
	int  j;
	Vec4 aux = {1, 0, 0, 0};
	
	for(j = 0; j < faces->used; j= j+3)//NOTAR QUE COMO GUARDA PARA CADA TRIANGULO LE CORRESPONDEN 3 LUGARES
    {   	   
	    Vec4 res1, res2, res3;
	    Vec3 v1, v2, v3;

	    Vec4 vertexA, vertexB, vertexC;
		CopyVec4(vertexA, vertices->array[faces->array[j][0]], 4);
		CopyVec4(vertexB, vertices->array[faces->array[j][1]], 4);
		CopyVec4(vertexC, vertices->array[faces->array[j][2]], 4);

	    Vec4Mat4ProductASM(vertexA, wvp, res1, aux);
	    v1[0] = (res1[0]/res1[3])*swidth/**0.5*/ + swidth*0.5 ;
	    v1[1] = (res1[1]/res1[3])*sheight/**0.5*/+ sheight*0.5; 
	    v1[2] = res1[2] / res1[3];

	    Vec4Mat4ProductASM(vertexB, wvp, res2, aux);
	    v2[0] = (res2[0]/res2[3])*swidth/**0.5*/ + swidth*0.5 ;
	    v2[1] = (res2[1]/res2[3])*sheight/**0.5*/ + sheight*0.5; 
	    v2[2] = res2[2] / res2[3];

	    Vec4Mat4ProductASM(vertexC, wvp, res3, aux);
	    v3[0] = (res3[0]/res3[3])*swidth/**0.5*/ + swidth*0.5 ;
	    v3[1] = (res3[1]/res3[3])*sheight/**0.5*/ + sheight*0.5; 
	 	v3[2] = res3[2] / res3[3];

		Vec3 color;
		color[0] = 1.0f;
		color[1] = color[0];
		color[2] = color[0];
		
		DrawTriangle_esq(v1, v2, v3, color, swidth, sheight, sf);
	}
}