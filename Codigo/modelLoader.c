#include "modelLoader.h"
#include <float.h>


bool LoadModel(char* path, VertexDynamicArray* outVertices, UInt3DynamicArray* outFaces)
{
	Vec2DynamicArray outUvs;
	outUvs.size = 0;
	Vec3DynamicArray outNormals;

	initVec3DynamicArray(&outNormals, 1);
	initVertexDynamicArray(outVertices, 1);
	initUInt3DynamicArray(outFaces, 1);

	FILE* file = fopen(path, "r");

	int texture = 0;

	if(file == NULL)
	{
		printf("Error al cargar elsadasd archivo.\n");
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
			Vertex v;
			ZeroVec(v.normal, 3);
			ZeroVec(v.wcoordinates, 3);
			ZeroVec(v.texCoordinates, 2);
			fscanf(file, "%f %f %f\n", &v.coordinates[0], &v.coordinates[1], &v.coordinates[2]);
			insertVertexDynamicArray(outVertices, &v);
		}
		else if(strcmp(lineHeader, "vt") == 0)
		{
			if(outUvs.size ==0) 
				initVec2DynamicArray(&outUvs, 1);

			Vec2 uv;
			fscanf(file, "%f %f\n", &uv[0], &uv[1]);
			insertVec2DynamicArray(&outUvs, uv);
		}
		else if(strcmp(lineHeader, "vn") == 0)
		{
			Vec3 normal;
			fscanf(file, "%f %f %f\n", &normal[0], &normal[1], &normal[2]);
			insertVec3DynamicArray(&outNormals, normal);
		}
		else if(strcmp(lineHeader, "f") == 0)
		{
			if(outUvs.size != 0 )
			{
				uint3 vertexIndex, uvIndex, normalIndex;
				int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], 
				&vertexIndex[1], &uvIndex[1], &normalIndex[1], 
				&vertexIndex[2], &uvIndex[2], &normalIndex[2]);

				if(matches != 9)
				{
					printf("Formato incompatible.\n");
					return false;
				}

				vertexIndex[0]--;
				vertexIndex[1]--;
				vertexIndex[2]--;
				normalIndex[0]--;
				normalIndex[1]--;
				normalIndex[2]--;
					
				CopyVec(outVertices->array[vertexIndex[0]].normal, outNormals.array[normalIndex[0]], 3);
				CopyVec(outVertices->array[vertexIndex[1]].normal, outNormals.array[normalIndex[1]], 3);
				CopyVec(outVertices->array[vertexIndex[2]].normal, outNormals.array[normalIndex[2]], 3);

				CopyVec(outVertices->array[vertexIndex[0]].texCoordinates, outUvs.array[uvIndex[0]], 2);
				CopyVec(outVertices->array[vertexIndex[1]].texCoordinates, outUvs.array[uvIndex[1]], 2);
				CopyVec(outVertices->array[vertexIndex[2]].texCoordinates, outUvs.array[uvIndex[2]], 2);
						
				insertUInt3DynamicArray(outFaces, vertexIndex);
			}
			else
			{
				uint3 vertexIndex, normalIndex;
				int matches = fscanf(file, "%d//%d %d//%d %d//%d\n", &vertexIndex[0], &normalIndex[0], 
				&vertexIndex[1], &normalIndex[1], 
				&vertexIndex[2], &normalIndex[2]);

				if(matches != 6)
				{
					printf("Formato incompatible.\n");
					return false;
				}

				vertexIndex[0]--;
				vertexIndex[1]--;
				vertexIndex[2]--;
				normalIndex[0]--;
				normalIndex[1]--;
				normalIndex[2]--;

				CopyVec(outVertices->array[vertexIndex[0]].normal, outNormals.array[normalIndex[0]], 3);
				CopyVec(outVertices->array[vertexIndex[1]].normal, outNormals.array[normalIndex[1]], 3);
				CopyVec(outVertices->array[vertexIndex[2]].normal, outNormals.array[normalIndex[2]], 3);
			
				insertUInt3DynamicArray(outFaces, vertexIndex);
			}
		}
	}

	if(outNormals.size != 0)
		freeVec3DynamicArray(&outNormals);

	if(outUvs.size != 0)
		freeVec2DynamicArray(&outUvs);
}


void RenderFilledModel(VertexDynamicArray* vertices, UInt3DynamicArray* faces, Mat4 wvp, int swidth, int sheight, SDL_Surface* sf, float* depthBuffer, Mat4 world, SDL_Surface* tex)
{
	int i, j, x0, x1, y0, y1;

	for(j = 0; j < faces->used; j++)
    {   	   
	    Vec4 res1, res2, res3, res1n, res2n, res3n, res1w, res2w, res3w;
	    Vec3 v1, v2, v3, v1n, v2n, v3n, v1w, v2w, v3w;

	    Vec3 vertexA, vertexB, vertexC;
		CopyVec(vertexA, vertices->array[faces->array[j][0]].coordinates, 3);
		CopyVec(vertexB, vertices->array[faces->array[j][1]].coordinates, 3);
		CopyVec(vertexC, vertices->array[faces->array[j][2]].coordinates, 3);

	    Vec3Mat4Product(vertexA, wvp, res1);
	    v1[0] = (res1[0]/res1[3])*swidth/**0.5*/ + swidth*0.5 ;
	    v1[1] = (res1[1]/res1[3])*sheight/**0.5*/+ sheight*0.5; 
	    v1[2] = res1[2] / res1[3];

	    Vec3Mat4Product(vertexB, wvp, res2);
	    v2[0] = (res2[0]/res2[3])*swidth/**0.5*/ + swidth*0.5 ;
	    v2[1] = (res2[1]/res2[3])*sheight/**0.5*/ + sheight*0.5; 
	    v2[2] = res2[2] / res2[3];

	    Vec3Mat4Product(vertexC, wvp, res3);
	    v3[0] = (res3[0]/res3[3])*swidth/**0.5*/ + swidth*0.5 ;
	    v3[1] = (res3[1]/res3[3])*sheight/**0.5*/ + sheight*0.5; 
	 	v3[2] = res3[2] / res3[3];

	 	///////////////////////////
 		Vec3Mat4Product(vertices->array[faces->array[j][0]].coordinates, world, res1w);
 		v1w[0] = res1w[0]; v1w[1] = res1w[1]; v1w[2] = res1w[2];

        Vec3Mat4Product(vertices->array[faces->array[j][1]].coordinates, world, res2w);
		v2w[0] = res2w[0]; v2w[1] = res2w[1]; v2w[2] = res2w[2];

        Vec3Mat4Product(vertices->array[faces->array[j][2]].coordinates, world, res3w);
        v3w[0] = res3w[0]; v3w[1] = res3w[1]; v3w[2] = res3w[2];        ////////////

        ////////////////////////////
        Vec3Mat4Product(vertices->array[faces->array[j][0]].normal, world, res1n);
        v1n[0] = res1n[0]; v1n[1] = res1n[1]; v1n[2] = res1n[2];        ////////////

        Vec3Mat4Product(vertices->array[faces->array[j][1]].normal, world, res2n);
		v2n[0] = res2n[0]; v2n[1] = res2n[1]; v2n[2] = res2n[2]; 

        Vec3Mat4Product(vertices->array[faces->array[j][2]].normal, world, res3n);
		v3n[0] = res3n[0]; v3n[1] = res3n[1]; v3n[2] = res3n[2];  

	   // Uint32 color =(j % faces->used);
		Vec3 color;
		color[0] = 0.25f + (j % faces->used) * 0.75f / faces->used;
		color[1] = color[0];
		color[2] = color[0];
	//	printf("%s\n", "color original");
//		printf("%f, %f, %f\n", color[0], color[1], color[2]);
		//sleep(1);

		DrawTriangle(v1, v2, v3, v1n, v2n, v3n, v1w, v2w, v3w, color, swidth, sheight, sf, depthBuffer, tex);
	}
}