#include "modelLoader.h"

bool LoadModel(char* path, Vec3DynamicArray* outVertices, Vec2DynamicArray* outUvs, Vec3DynamicArray* outNormals, UInt3DynamicArray* outFaces)
{
	FILE* file = fopen(path, "r");

	initVec3DynamicArray(outVertices, 1);
	initVec2DynamicArray(outUvs, 1);
	initVec3DynamicArray(outNormals, 1);
	initUInt3DynamicArray(outFaces, 1);

	if(file == NULL)
	{
		printf("Error al cargar el archivo.\n");
		return false;
	}

	while(1)
	{
		//asumimos que la primera linea no va a tener mas de 128 caracteres
		char lineHeader[128];

		int res = fscanf(file, "%s", lineHeader);
		if(res == EOF)
			break;

		if(strcmp(lineHeader, "v") == 0)
		{
			Vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex[0], &vertex[1], &vertex[2]);
			insertVec3DynamicArray(outVertices, vertex);
			//printf("mostrando used: %f %f %f\n", outVertices->array[outVertices->used -1][0], outVertices->array[outVertices->used -1][1], outVertices->array[outVertices->used -1][2]);
		}
		else if(strcmp(lineHeader, "vt") == 0)
		{
			Vec2 uv;
			fscanf(file, "%f %f\n", &uv[0], &uv[1]);
			insertVec2DynamicArray(outUvs, uv);
			//printf("mostrando used: %f %f\n", outUvs->array[outUvs->used -1][0], outUvs->array[outUvs->used -1][1]);
		}
		else if(strcmp(lineHeader, "vn") == 0)
		{
			Vec3 normal;
			fscanf(file, "%f %f %f\n", &normal[0], &normal[1], &normal[2]);
			insertVec3DynamicArray(outNormals, normal);
			//printf("mostrando used: %f %f %f\n", outNormals->array[outNormals->used -1][0], outNormals->array[outNormals->used -1][1], outNormals->array[outNormals->used -1][2]);
		}
		else if(strcmp(lineHeader, "f") == 0)
		{
			uint3 vertexIndex, uvIndex, normalIndex;
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], 
				&vertexIndex[1], &uvIndex[1], &normalIndex[1], 
				&vertexIndex[2], &uvIndex[2], &normalIndex[2]);

			vertexIndex[0]--;
			vertexIndex[1]--;
			vertexIndex[2]--;

			if(matches != 9)
			{
				printf("Formato incompatible.\n");
				return false;
			}

			insertUInt3DynamicArray(outFaces, vertexIndex);
			printf("mostrando used: %d %d %d\n", outFaces->array[outFaces->used -1][0], outFaces->array[outFaces->used -1][1], outFaces->array[outFaces->used -1][2]);
		}
	}
}

void RenderModel(Vec3DynamicArray* vertices, UInt3DynamicArray* faces, Vec2DynamicArray* uvs, Mat4 wvp, uint swidth, uint sheight, SDL_Surface* screenSurface)
{
	int i, j, x0, x1, y0, y1;

	for(j = 0; j < faces->used; j++)
	{
		for(i = 0; i < 2; i++)
		{
			Vec4 res;
            Vec3Mat4Product(vertices->array[faces->array[j][i]], wvp, res);
            x0 = (res[0]/res[3])*swidth*0.5 + swidth*0.5 ;
			y0 = (res[1]/res[3])*sheight*0.5 + sheight*0.5; 
  		 	
			Vec3Mat4Product(vertices->array[faces->array[j][i+1]], wvp, res);
			x1 = (res[0]/res[3])*swidth*0.5 + swidth*0.5;
			y1 = (res[1]/res[3])*sheight*0.5 + sheight*0.5;

			DrawBline(  x0,  x1,  y0,  y1, screenSurface);
		}

		Vec4 res;

        Vec3Mat4Product(vertices->array[faces->array[j][2]], wvp, res);
        x0 = (res[0]/res[3])*swidth*0.5 + swidth*0.5 ;
		y0 = (res[1]/res[3])*sheight*0.5 + sheight*0.5; 
		 	
		Vec3Mat4Product(vertices->array[faces->array[j][0]], wvp, res);
		x1 = (res[0]/res[3])*swidth*0.5 + swidth*0.5 ;
		y1 = (res[1]/res[3])*sheight*0.5 + sheight*0.5;

		DrawBline(  x0,  x1,  y0,  y1, screenSurface);
	}
}