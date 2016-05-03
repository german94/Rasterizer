#include "modelLoader.h"
#include <float.h>

bool ESCALADO = false;
float factor;

bool LoadModel(char* path, Model* data)
{

	initVec4DynamicArray(data->normals, 0);
	
	initUInt3DynamicArray(data->faces, 1);
	
	initVec4DynamicArray(data->vertices, 1);

	initVec2DynamicArray(data->uvs, 0);

	FILE* file = fopen(path, "r");

	if(file == NULL)
	{
		printf("Error al cargar el archivo.\n");
		return false;
	}
	float maxy = -FLT_MAX; float maxx = -FLT_MAX;
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
				data->max_dis[0] = indice; 
			}
			if(v[1] >= maxy)
			{	
				maxy = v[1];	
				data->max_dis[1] = indice;  
			}	

			insertVec4DynamicArray(data->vertices, v);
			indice++;

		}
		else if(strcmp(lineHeader, "vt") == 0)
		{	
			if(data->uvs->size ==0) 
				initVec2DynamicArray(data->uvs, 1);

			Vec2 uv;
			fscanf(file, "%f %f\n", &uv[0], &uv[1]);
			uv[1] = 1 - uv[1];
			insertVec2DynamicArray(data->uvs, uv);
		}
		else if(strcmp(lineHeader, "vn") == 0)
		{
			if(data->normals->size ==0) 
				initVec4DynamicArray(data->normals, 1);

			Vec4 normal;
			fscanf(file, "%f %f %f\n", &normal[0], &normal[1], &normal[2]); normal[3] = 1.0f;
			insertVec4DynamicArray(data->normals, normal);
		}
		else if(strcmp(lineHeader, "f") == 0)
		{	
			int matches, cero_a, cero_b, cero_c, uno_a, uno_b, uno_c, dos_a, dos_b, dos_c, tres_a, tres_b, tres_c; 
			
			if(data->uvs->size != 0 && data->normals->size !=0)
			{
				matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d\n", &cero_a, &cero_b, &cero_c, &uno_a, &uno_b, &uno_c, &dos_a, &dos_b, &dos_c, &tres_a, &tres_b, &tres_c); 
				if(matches != 12 && matches != 9)
				{
					printf("Formato incompatible.\n");
					return false;
				}	
			}
			if(data->uvs->size == 0 && data->normals->size !=0)
			{
				matches = fscanf(file, "%d//%d %d//%d %d//%d %d//%d\n", &cero_a, &cero_c, &uno_a, &uno_c, &dos_a, &dos_c, &tres_a, &tres_c);
				if(matches != 6 && matches!=8)
				{
					printf("Formato incompatible.\n");
					return false;
				}
			}
			if(data->uvs->size != 0 && data->normals->size ==0)
			{
				matches = fscanf(file, "%d/%d %d/%d %d/%d %d/%d\n", &cero_a, &cero_b, &uno_a, &uno_b, &dos_a, &dos_b, &tres_a, &tres_b);
				if(matches != 6 && matches!=8)
				{
					printf("Formato incompatible.\n");
					return false;
				}
			}
			if(data->uvs->size == 0 && data->normals->size == 0)
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
			insertUInt3DynamicArray(data->faces, vertexIndex);
			insertUInt3DynamicArray(data->faces, uvIndex);
			insertUInt3DynamicArray(data->faces, normalIndex);

			if(matches== 8 || matches == 12 || matches == 4)
			{
				vertexIndex[0] = cero_a ; vertexIndex[1] = dos_a; vertexIndex[2] = tres_a;
				uvIndex[0] =  cero_b; uvIndex[1] = dos_b; uvIndex[2] = tres_b;	
				normalIndex[0] =  cero_c; normalIndex[1] = dos_c; normalIndex[2] = tres_c;
				insertUInt3DynamicArray(data->faces, vertexIndex);
				insertUInt3DynamicArray(data->faces, uvIndex);
				insertUInt3DynamicArray(data->faces, normalIndex);
			}
		}
	}
}
    
void escalando(Model* data, int swidth, int sheight)
{
	Vec4 maxx, maxy, resx, resy;
	float escalado_x, escalado_y;
	CopyVec(maxx, data->vertices->array[data->max_dis[0]], 4);
	CopyVec(maxy, data->vertices->array[data->max_dis[1]], 4);
	  
	Vec4Mat4Product(maxx, data->wvp, resx);
	float x_final = (resx[0]/resx[3])*swidth + swidth*0.5 ;
	if (x_final < swidth*0.7)
		escalado_x = 1.0;
	else
		escalado_x = (swidth*0.7)/x_final;
	
	if(x_final < swidth*0.6)
		escalado_x = 1.7;
	
	Vec4Mat4Product(maxy, data->wvp, resy);
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

void RenderFilledModel_m3(Model* data, SDL_Surface* sf, float* depthBuffer, SDL_Surface* tex,  Vec3 lightPos)
{	
	int  j;
	if (!ESCALADO)
    	escalando(data, sf->w, sf->h);
	for(j = 0; j < data->faces->used; j= j+3)//NOTAR QUE COMO GUARDA PARA CADA TRIANGULO LE CORRESPONDEN 3 LUGARES
    {   	   
	    Vec4 res1, res2, res3;
	    Vec3 v1, v2, v3;

	    Vec4 vertexA, vertexB, vertexC;
	    Vertex p1, p2, p3;
		CopyVec(vertexA, data->vertices->array[data->faces->array[j][0]], 4);
		CopyVec(vertexB, data->vertices->array[data->faces->array[j][1]], 4);
		CopyVec(vertexC, data->vertices->array[data->faces->array[j][2]], 4);
		VecByScalar(vertexA, factor, vertexA, 3);
        VecByScalar(vertexB, factor, vertexB, 3);
        VecByScalar(vertexC, factor, vertexC, 3);


	    Vec4Mat4Product(vertexA, data->wvp, res1);
	    v1[0] = (res1[0]/res1[3])*sf->w + sf->w*0.5 ;
	    v1[1] = (-res1[1]/res1[3])*sf->h+ sf->h*0.7; 
	    v1[2] = res1[2] / res1[3];

	    Vec4Mat4Product(vertexB, data->wvp, res2);
	    v2[0] = (res2[0]/res2[3])*sf->w + sf->w*0.5 ;
	    v2[1] = (-res2[1]/res2[3])*sf->h + sf->h*0.7; 
	    v2[2] = res2[2] / res2[3];

	    Vec4Mat4Product(vertexC, data->wvp, res3);
	    v3[0] = (res3[0]/res3[3])*sf->w + sf->w*0.5 ;
	    v3[1] = (-res3[1]/res3[3])*sf->h + sf->h*0.7; 
	 	v3[2] = res3[2] / res3[3];

	 	CopyVec(p1.coordinates, v1, 3);
	 	CopyVec(p2.coordinates, v2, 3);
	 	CopyVec(p3.coordinates, v3, 3);

	 	///////////////////////////
 		Vec4Mat4Product(vertexA, data->world, p1.wcoordinates);

        Vec4Mat4Product(vertexB, data->world, p2.wcoordinates);

        Vec4Mat4Product(vertexC, data->world, p3.wcoordinates);

        ////////////////////////////

        Vec4 vertexAn, vertexBn, vertexCn;
		CopyVec(vertexAn, data->normals->array[data->faces->array[j+2][0]], 4);
		CopyVec(vertexBn, data->normals->array[data->faces->array[j+2][1]], 4);
		CopyVec(vertexCn, data->normals->array[data->faces->array[j+2][2]], 4);

        Vec4Mat4Product(vertexAn, data->world, p1.normal);
        Vec4Mat4Product(vertexBn, data->world, p2.normal);
        Vec4Mat4Product(vertexCn, data->world, p3.normal);

		if(tex != NULL)
		{
			CopyVec(p1.texCoordinates, data->uvs->array[data->faces->array[j+1][0]], 2);
			CopyVec(p2.texCoordinates, data->uvs->array[data->faces->array[j+1][1]], 2);
			CopyVec(p3.texCoordinates, data->uvs->array[data->faces->array[j+1][2]], 2);
		}
	
		if(v1[1] <= v2[1] && v2[1] <= v3[1])
			DrawTriangle_m3(data->color, &p1, &p2, &p3, sf, depthBuffer, tex, lightPos);
		else if(v1[1] <= v3[1] && v3[1] <= v2[1])
			DrawTriangle_m3(data->color, &p1, &p3, &p2, sf, depthBuffer, tex, lightPos);
		else if(v2[1] <= v1[1] && v1[1] <= v3[1])
			DrawTriangle_m3(data->color, &p2, &p1, &p3, sf, depthBuffer, tex, lightPos);
		else if(v2[1] <= v3[1] && v3[1] <= v1[1])
			DrawTriangle_m3(data->color, &p2, &p3, &p1, sf, depthBuffer, tex, lightPos);
		else if(v3[1] <= v2[1] && v2[1] <= v1[1])
			DrawTriangle_m3(data->color, &p3, &p2, &p1, sf, depthBuffer, tex, lightPos);
		else if(v3[1] <= v1[1] && v1[1] <= v2[1])
			DrawTriangle_m3(data->color, &p3, &p1, &p2, sf, depthBuffer, tex, lightPos);
	}
}

void RenderFilledModel_m2(Model* data, SDL_Surface* sf, float* depthBuffer, SDL_Surface* tex)
{	
	int  j;
	if (!ESCALADO)
    	escalando(data, sf->w, sf->h);
	for(j = 0; j < data->faces->used; j= j+3)//NOTAR QUE COMO GUARDA PARA CADA TRIANGULO LE CORRESPONDEN 3 LUGARES
    {   	   
	    Vec4 res1, res2, res3;
	    Vec3 v1, v2, v3;
	    Vertex p1, p2, p3;

	    Vec4 vertexA, vertexB, vertexC;
		CopyVec(vertexA, data->vertices->array[data->faces->array[j][0]], 4);
		CopyVec(vertexB, data->vertices->array[data->faces->array[j][1]], 4);
		CopyVec(vertexC, data->vertices->array[data->faces->array[j][2]], 4);
		VecByScalar(vertexA, factor, vertexA, 3);
        VecByScalar(vertexB, factor, vertexB, 3);
        VecByScalar(vertexC, factor, vertexC, 3);


	    Vec4Mat4Product(vertexA, data->wvp, res1);
	    v1[0] = (res1[0]/res1[3])*sf->w + sf->w*0.5 ;
	    v1[1] = (-res1[1]/res1[3])*sf->h+ sf->h*0.7; 
	    v1[2] = res1[2] / res1[3];

	    Vec4Mat4Product(vertexB, data->wvp, res2);
	    v2[0] = (res2[0]/res2[3])*sf->w + sf->w*0.5 ;
	    v2[1] = (-res2[1]/res2[3])*sf->h + sf->h*0.7; 
	    v2[2] = res2[2] / res2[3];

	    Vec4Mat4Product(vertexC, data->wvp, res3);
	    v3[0] = (res3[0]/res3[3])*sf->w + sf->w*0.5 ;
	    v3[1] = (-res3[1]/res3[3])*sf->h + sf->h*0.7; 
	 	v3[2] = res3[2] / res3[3];

	 	CopyVec(p1.coordinates, v1, 3);
	 	CopyVec(p2.coordinates, v2, 3);
	 	CopyVec(p3.coordinates, v3, 3);

	 	///////////////////////////
		if(tex != NULL)
		{
			CopyVec(p1.texCoordinates, data->uvs->array[data->faces->array[j+1][0]], 2);
			CopyVec(p2.texCoordinates, data->uvs->array[data->faces->array[j+1][1]], 2);
			CopyVec(p3.texCoordinates, data->uvs->array[data->faces->array[j+1][2]], 2);
		}

		if(v1[1] <= v2[1] && v2[1] <= v3[1])
			DrawTriangle_m2(data->color, &p1, &p2, &p3, sf, depthBuffer, tex);
		else if(v1[1] <= v3[1] && v3[1] <= v2[1])
			DrawTriangle_m2(data->color, &p1, &p3, &p2, sf, depthBuffer, tex);
		else if(v2[1] <= v1[1] && v1[1] <= v3[1])
			DrawTriangle_m2(data->color, &p2, &p1, &p3, sf, depthBuffer, tex);
		else if(v2[1] <= v3[1] && v3[1] <= v1[1])
			DrawTriangle_m2(data->color, &p2, &p3, &p1, sf, depthBuffer, tex);
		else if(v3[1] <= v2[1] && v2[1] <= v1[1])
			DrawTriangle_m2(data->color, &p3, &p2, &p1, sf, depthBuffer, tex);
		else if(v3[1] <= v1[1] && v1[1] <= v2[1])
			DrawTriangle_m2(data->color, &p3, &p1, &p2, sf, depthBuffer, tex);
	}
}


void RenderFilledModel_m1(Model* data, SDL_Surface* sf)
{	
	int  j;
	if (!ESCALADO)
    	escalando(data, sf->w, sf->h);
	for(j = 0; j < data->faces->used; j= j+3)//NOTAR QUE COMO GUARDA PARA CADA TRIANGULO LE CORRESPONDEN 3 LUGARES
    {   	   
	    Vec4 res1, res2, res3;
	    Vec3 v1, v2, v3;

	    Vec4 vertexA, vertexB, vertexC;
		CopyVec(vertexA, data->vertices->array[data->faces->array[j][0]], 4);
		CopyVec(vertexB, data->vertices->array[data->faces->array[j][1]], 4);
		CopyVec(vertexC, data->vertices->array[data->faces->array[j][2]], 4);
		VecByScalar(vertexA, factor, vertexA, 3);
        VecByScalar(vertexB, factor, vertexB, 3);
        VecByScalar(vertexC, factor, vertexC, 3);

	    Vec4Mat4Product(vertexA, data->wvp, res1);
	    v1[0] = (res1[0]/res1[3])*sf->w + sf->w*0.5 ;
	    v1[1] = (-res1[1]/res1[3])*sf->h+ sf->h*0.7; 
	    v1[2] = res1[2] / res1[3];

	    Vec4Mat4Product(vertexB, data->wvp, res2);
	    v2[0] = (res2[0]/res2[3])*sf->w + sf->w*0.5 ;
	    v2[1] = (-res2[1]/res2[3])*sf->h + sf->h*0.7; 
	    v2[2] = res2[2] / res2[3];

	    Vec4Mat4Product(vertexC, data->wvp, res3);
	    v3[0] = (res3[0]/res3[3])*sf->w + sf->w*0.5 ;
	    v3[1] = (-res3[1]/res3[3])*sf->h + sf->h*0.7; 
	 	v3[2] = res3[2] / res3[3];

		DrawTriangle_m1(v1, v2, v3, sf);
	}
}