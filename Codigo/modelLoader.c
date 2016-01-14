#include "modelLoader.h"

bool LoadModel(char* path, Vec3DynamicArray* outVertices, Vec2DynamicArray* outUvs, Vec3DynamicArray* outNormals, UInt3DynamicArray* outFaces)
{
	FILE* file = fopen(path, "r");

	initVec3DynamicArray(outVertices, 0);
	initVec2DynamicArray(outUvs, 0);
	initVec3DynamicArray(outNormals, 0);
	initUInt3DynamicArray(outFaces, 0);

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
		}
		else if(strcmp(lineHeader, "vt") == 0)
		{
			Vec2 uv;
			fscanf(file, "%f %f\n", &uv[0], &uv[1]);
			insertVec2DynamicArray(outUvs, uv);
		}
		else if(strcmp(lineHeader, "vn") == 0)
		{
			Vec3 normal;
			fscanf(file, "%f %f %f\n", &normal[0], &normal[1], &normal[2]);
			insertVec3DynamicArray(outNormals, normal);
		}
		else if(strcmp(lineHeader, "f") == 0)
		{
			uint3 vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], 
				&vertexIndex[1], &uvIndex[1], &normalIndex[1], 
				&vertexIndex[2], &uvIndex[2], &normalIndex[2]);

			if(matches != 9)
			{
				printf("Formato incompatible.\n");
				return false;
			}

			insertUInt3DynamicArray(outFaces, vertexIndex);
		}
	}
}