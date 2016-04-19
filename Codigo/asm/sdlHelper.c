#include "sdlHelper.h"

extern void InterpolateASM(float *gradientes, float *max, float *min, float *unos, float *res);

void putpixel(SDL_Surface *surface, int x, int y, float z, Uint32 pixel, int swidth, int sheight, float* depthBuffer)
{
	
	int index = x + y*swidth;
	if(depthBuffer[index] > z)
	{
  		depthBuffer[index] = z;
	    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to set */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        *p = pixel;
        break;

    case 2:
        *(Uint16 *)p = pixel;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            p[0] = (pixel >> 16) & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = pixel & 0xff;
        } else {
            p[0] = pixel & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = (pixel >> 16) & 0xff;
        }
        break;

    case 4:
        *(Uint32 *)p = pixel;
        break;
    }
	

	}
}

Uint32 getpixel(SDL_Surface *surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        return *p;
        break;

    case 2:
        return *(Uint16 *)p;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;
        break;

    case 4:
        return *(Uint32 *)p;
        break;

    default:
        return 0;       /* shouldn't happen, but avoids warnings */
    }
}

float Interpolate1(float min, float max, float gradient)
{ 
    float mini = (gradient < 1) ? gradient : 1;
    float maxi = (0 > mini) ? 0 : mini;
    return min + (max - min) *maxi;
}

void RenderText(SDL_Color clrFg, TTF_Font* font, char* buf, SDL_Surface* screenSurface)
{
    SDL_Surface *sText = TTF_RenderText_Solid( font, buf, clrFg );

    SDL_Rect rcDest = {0,0,0,0};

    SDL_BlitSurface( sText,NULL, screenSurface,&rcDest );

    SDL_FreeSurface( sText );
}

void RenderTextR(SDL_Color clrFg, TTF_Font* font, char* buf, SDL_Surface* screenSurface, SDL_Rect* r)
{
    SDL_Surface *sText = TTF_RenderText_Solid( font, buf, clrFg );

    SDL_BlitSurface(sText, NULL, screenSurface, r);

    SDL_FreeSurface( sText );
}


void DrawPoint(int x, int y, float z, float* depthBuffer, int SW, int SH, SDL_Surface* sf, Uint32 color)
{
    if(x >= 0 && y >= 0 && x < SW && y <SH)
        putpixel(sf, x, y, z, color, SW, SH, depthBuffer);
}

float ComputeNDotL(Vec3 centerPoint, Vec3 vnFace, Vec3 lightPos) 
{
	Vec3 lightDirection;

	Sub3(lightPos, centerPoint, lightDirection);

	Normalize3(vnFace);
  
	Normalize3(lightDirection);

	float prod = Dot3Prod(vnFace, lightDirection);

	return (0 > prod) ? 0 : prod;
}

Uint32 Map(SDL_Surface* tex, float tu, float tv)
{
    if (tex == NULL) 
        return 0;
    
    int u = abs((int) (tu*tex->w) % tex->w);
    int v = abs((int) (tv*tex->h) % tex->h);

    return getpixel(tex, u, v);
}

void Uint32ToVec4(Uint32 inColor, Vec3 outColor) //recibo (abgr)//REVISAR ORDEN CON LA ORIGINAL
{
    //outColor[0] = ((inColor >> 24) & 255) / 255.0f;// a
    outColor[0] = ((inColor >> 16) & 255) / 255.0f;// b
    outColor[1] = ((inColor >> 8) & 255) / 255.0f;// g
    outColor[2] = (inColor & 255) / 255.0f; // r 
}//devuelvo (b,g,r)


Uint32 Vec4ToUint32P(float _r, float _g, float _b)
{
    Uint32 r = (Uint32)(_r * 255.0f) & 255;
    Uint32 g = (Uint32)(_g * 255.0f) & 255;
    Uint32 b = (Uint32)(_b * 255.0f) & 255;
    //Uint32 b = (Uint32)(_a * 255.0f) & 255;
    Uint32 value = r;
    value |= g << 8;
    value |= b << 16;
   // value |= a << 24;
    return (Uint32)value;
}

void ProcessScanLine(Vertex* va, Vertex* vb, Vertex* vc, Vertex* vd, Vec3 color, int SW, int SH, SDL_Surface* sf, float* depthBuffer, ScanLineData* data,  SDL_Surface* tex)
{
    // Thanks to current Y, we can compute the gradient to compute others values like
    // the starting X (sx) and ending X (ex) to draw between
    // if pa.Y == pb.Y or pc.Y == pd.Y, gradient is forced to 1

    Vec3 pa, pb, pc, pd;
    CopyVec(pa, va->coordinates, 3);
    CopyVec(pb, vb->coordinates, 3);
    CopyVec(pc, vc->coordinates, 3);
    CopyVec(pd, vd->coordinates, 3);

	Vec4 min = { pa[0], pc[0], pa[2], pc[2]};
    Vec4 max = { pb[0],pd[0], pb[2], pd[2]};
    Vec4 unos = {1.0, 1.0, 1.0, 1.0};
	Vec4 res;

    float gradient1 = pa[1] != pb[1] ? (data->currentY - pa[1]) / (pb[1] - pa[1]) : 1;
    float gradient2 = pc[1] != pd[1] ? (data->currentY - pc[1]) / (pd[1] - pc[1]) : 1;
    Vec4 gradientes = {gradient1, gradient2, gradient1, gradient2};
 	
    InterpolateASM(gradientes, max, min, unos, res);
    int sx = (int)res[0];
    int ex = (int)res[1];
    float z1 = res[2];
    float z2 = res[3];

    min[0] = data->ndotla ; min[1] = data->ndotlc ; min[2] = va->texCoordinates[0] ; min[3] = vc->texCoordinates[0] ;
    max[0] = data->ndotlb ; max[1] = data->ndotld ; max[2] = vb->texCoordinates[0] ; max[3] = vd->texCoordinates[0] ;
    InterpolateASM(gradientes, max, min, unos, res);
    float snl = res[0]; 
	float enl = res[1];
	float su = res[2];
	float eu = res[3];

    min[0] = va->texCoordinates[1] ; min[1] = vc->texCoordinates[1] ;
	max[0] = vb->texCoordinates[1] ; max[1] = vd->texCoordinates[1] ;
    InterpolateASM(gradientes, max, min, unos, res);
    float sv = res[0];
    float ev = res[1];

    min[0] = z1; min[1] = snl; min[2] = su; min[3] = sv;
	max[0] = z2; max[1] = enl; max[2] = eu; max[3] = ev; 

    int x;
    for (x = sx ; x < ex; x++)
    {
  		float gradient = (x - sx) / (float)(ex - sx);
  		gradientes[0] = gradient; gradientes[1] = gradient; gradientes[2] = gradient; gradientes[3] = gradient;
        
        InterpolateASM(gradientes, max, min, unos, res);

        float z = res[0];
        float ndotl = res[1];
        float u = res[2];
        float v = res[3];

        Uint32 textureColor;

        if (tex != NULL)
            textureColor = Map(tex, u, v);
        else
            textureColor = 0xfffffff;      //si no hay textura entonces pongo  255 

        Vec3 texColor;
        Uint32ToVec4(textureColor, texColor);
        VecByVec(texColor, color, texColor, 3);
        VecByScalar(texColor, ndotl, texColor, 3);

        textureColor = Vec4ToUint32P(texColor[2], texColor[1], texColor[0]);

        DrawPoint(x, data->currentY, z, depthBuffer, SW, SH, sf, textureColor);
    }
        
}

void DrawTriangle(Vec3 p1, Vec3 p2, Vec3 p3, Vec3 p1n, Vec3 p2n, Vec3 p3n, Vec3 p1w, Vec3 p2w, Vec3 p3w, Vec3 color, int SW, int SH, SDL_Surface* sf, float* depthBuffer,  SDL_Surface* tex, Vec2 p1t, Vec2 p2t, Vec2 p3t )
{
    // Sorting the points in order to always have this order on screen p1, p2 & p3
    // with p1 always up (thus having the Y the lowest possible to be near the top screen)
    // then p2 between p1 & p3
    Vec3 temp, tempn, tempw;
    Vec2 tempt;
    if (p1[1] > p2[1])
    {
     	CopyVec(temp, p2, 3);
     	CopyVec(p2, p1, 3);
     	CopyVec(p1, temp, 3);
        CopyVec(tempn, p2n, 3);
        CopyVec(p2n, p1n, 3);
        CopyVec(p1n, tempn, 3);
        CopyVec(tempw, p2w, 3);
        CopyVec(p2w, p1w, 3);
        CopyVec(p1w, tempw, 3);
        if(tex != NULL){
        	CopyVec(tempt, p2t, 2);
       		CopyVec(p2t, p1t, 2);
        	CopyVec(p1t, tempt, 2);}
    }

    if (p2[1] > p3[1])
    {
     	CopyVec(temp, p2, 3);
     	CopyVec(p2, p3, 3);
     	CopyVec(p3, temp, 3);
        CopyVec(tempn, p2n, 3);
        CopyVec(p2n, p3n, 3);
        CopyVec(p3n, tempn, 3);
        CopyVec(tempw, p2w, 3);
        CopyVec(p2w, p3w, 3);
        CopyVec(p3w, tempw, 3);
        if(tex != NULL){
        	CopyVec(tempt, p2t, 2);
        	CopyVec(p2t, p3t, 2);
        	CopyVec(p3t, tempt, 2);}
    }

    if (p1[1] > p2[1])
    {
     	CopyVec(temp, p2, 3);
     	CopyVec(p2, p1, 3);
     	CopyVec(p1, temp, 3);
        CopyVec(tempn, p2n, 3);
        CopyVec(p2n, p1n, 3);
        CopyVec(p1n, tempn, 3);
        CopyVec(tempw, p2w, 3);
        CopyVec(p2w, p1w, 3);
        CopyVec(p1w, tempw, 3);
        if(tex != NULL){
        	CopyVec(tempt, p2t, 2);
        	CopyVec(p2t, p1t, 2);
        	CopyVec(p1t, tempt, 2);}
    }

    Vec3 lightPos;
    lightPos[0] = 0; lightPos[1] = 0; lightPos[2] = 20;

    float nl1 = ComputeNDotL(p1w, p1n, lightPos);
    float nl2 = ComputeNDotL(p2w, p2n, lightPos);
    float nl3 = ComputeNDotL(p3w, p3n, lightPos);

    // inverse slopes
    float dP1P2, dP1P3;

    // Computing inverse slopes
    if (p2[1] - p1[1] > 0)
    	dP1P2 = (p2[0] - p1[0]) / (p2[1] - p1[1]);//cheque la pendiente 
    else
    	dP1P2 = 0;

    if (p3[1] - p1[1] > 0)
    	dP1P3 = (p3[0] - p1[0]) / (p3[1] - p1[1]);
    else
    	dP1P3 = 0;

    
    Vertex v1, v2, v3;
    CopyVec(v1.coordinates, p1, 3);
    CopyVec(v2.coordinates, p2, 3);
    CopyVec(v3.coordinates, p3, 3);
    if(tex != NULL)
    	CopyVec(v1.texCoordinates, p1t, 2);
    	CopyVec(v2.texCoordinates, p2t, 2);
    	CopyVec(v3.texCoordinates, p3t, 2);



    if(p1[1] != p2[1])
    {  
    	if (dP1P2 > dP1P3)
    	{
        	ScanLineData data;
            int y;
        	for ( y = (int)p1[1]; y <= (int)p3[1]; y++)
        	{
                data.currentY = y;

        		if (y < p2[1])
                {
                    data.ndotla = nl1;
                    data.ndotlb = nl3;
                    data.ndotlc = nl1;
                    data.ndotld = nl2;
                    ProcessScanLine(&v1, &v3, &v1, &v2, color, SW, SH, sf, depthBuffer, &data, tex);
                }    
            	else
            	{
                    data.ndotla = nl1;
                    data.ndotlb = nl3;
                    data.ndotlc = nl2;
                    data.ndotld = nl3;
                    ProcessScanLine(&v1, &v3, &v2, &v3, color, SW, SH, sf, depthBuffer, &data, tex);
                }	
        	}
		}
    	else
    	{
        	ScanLineData data;
            int y;
        	for (y = (int)p1[1]; y <= (int)p3[1]; y++)
        	{
                data.currentY = y;

            	if (y < p2[1])
        		{
                    data.ndotla = nl1;
                    data.ndotlb = nl2;
                    data.ndotlc = nl1;
                    data.ndotld = nl3;
                    ProcessScanLine(&v1, &v2, &v1, &v3, color, SW, SH, sf, depthBuffer, &data, tex);
                }
            	else
        		{
                    data.ndotla = nl2;
                    data.ndotlb = nl3;
                    data.ndotlc = nl1;
                    data.ndotld = nl3;
                    ProcessScanLine(&v2, &v3, &v1, &v3, color, SW, SH, sf, depthBuffer, &data, tex);
                }
        	}
    	}
    }
    else
    {
    	if (p1[0] < p2[0])
    	{
        	ScanLineData data;
            int y;
        	for ( y = (int)p1[1]; y <= (int)p3[1]; y++)
        	{
        		data.currentY = y;

                data.ndotla = nl1;//REVISAR ESTOS NDOTL
                data.ndotlb = nl3;
                data.ndotlc = nl2;
                data.ndotld = nl3;
                ProcessScanLine(&v1, &v3, &v2, &v3, color, SW, SH, sf, depthBuffer, &data, tex);
               
        	}
        }
    	else
    	{
        	ScanLineData data;
            int y;
        	for (y = (int)p1[1]; y <= (int)p3[1]; y++)
        	{
                data.currentY = y;

                data.ndotla = nl2;//REVISAR ESTOS NDOTL
                data.ndotlb = nl3;
                data.ndotlc = nl1;
                data.ndotld = nl3;
                ProcessScanLine(&v2, &v3, &v1, &v3, color, SW, SH, sf, depthBuffer, &data, tex);
        	}
        }
    } 
}


void ProcessScanLine_tex(Vertex* va, Vertex* vb, Vertex* vc, Vertex* vd, Vec3 color, int SW, int SH, SDL_Surface* sf, float* depthBuffer, ScanLineData* data,  SDL_Surface* tex)
{
    // Thanks to current Y, we can compute the gradient to compute others values like
    // the starting X (sx) and ending X (ex) to draw between
    // if pa.Y == pb.Y or pc.Y == pd.Y, gradient is forced to 1

    Vec3 pa, pb, pc, pd;
    CopyVec(pa, va->coordinates, 3);
    CopyVec(pb, vb->coordinates, 3);
    CopyVec(pc, vc->coordinates, 3);
    CopyVec(pd, vd->coordinates, 3);

    Vec4 min = { pa[0], pc[0], pa[2], pc[2]};
    Vec4 max = { pb[0],pd[0], pb[2], pd[2]};
    Vec4 unos = {1.0, 1.0, 1.0, 1.0};
	Vec4 res;

    float gradient1 = pa[1] != pb[1] ? (data->currentY - pa[1]) / (pb[1] - pa[1]) : 1;
    float gradient2 = pc[1] != pd[1] ? (data->currentY - pc[1]) / (pd[1] - pc[1]) : 1;
    Vec4 gradientes = {gradient1, gradient2, gradient1, gradient2};
    
    InterpolateASM(gradientes, max, min, unos, res);
    int sx = (int)res[0];
    int ex = (int)res[1];
    float z1 = res[2];
    float z2 = res[3];     
   
    min[0] = va->texCoordinates[0] ; min[1] = vc->texCoordinates[0] ; min[2] = va->texCoordinates[1] ; min[3] = vc->texCoordinates[1] ;
    max[0] = vb->texCoordinates[0] ; max[1] = vd->texCoordinates[0] ; max[2] = vb->texCoordinates[1] ; max[3] = vd->texCoordinates[1] ;
    InterpolateASM(gradientes, max, min, unos, res);
    float su = res[0]; 
	float eu = res[1];
	float sv = res[2];
	float ev = res[3];

	min[0] = z1; min[1] = su; min[2] = sv;
	max[0] = z2; max[1] = eu; max[2] = ev;  

    int x;
    for (x = sx ; x < ex; x++)
    {
        float gradient = (x - sx) / (float)(ex - sx);

        gradientes[0] = gradient; gradientes[1] = gradient; gradientes[2] = gradient;
        InterpolateASM(gradientes, max, min, unos, res);

        float z = res[0];
        float u = res[1];
        float v = res[2];

        Uint32 textureColor;

        if (tex != NULL)
            textureColor = Map(tex, u, v);
        else
            textureColor = 0xfffffff;      //si no hay textura entonces pongo  255 

        Vec3 texColor;
        Uint32ToVec4(textureColor, texColor);
        VecByVec(texColor, color, texColor, 3);

        textureColor = Vec4ToUint32P(texColor[2], texColor[1], texColor[0]);

        DrawPoint(x, data->currentY, z, depthBuffer, SW, SH, sf, textureColor);
    }
        
}

void DrawTriangle_tex(Vec3 p1, Vec3 p2, Vec3 p3, Vec3 color, int SW, int SH, SDL_Surface* sf, float* depthBuffer,  SDL_Surface* tex, Vec2 p1t, Vec2 p2t, Vec2 p3t )
{
    // Sorting the points in order to always have this order on screen p1, p2 & p3
    // with p1 always up (thus having the Y the lowest possible to be near the top screen)
    // then p2 between p1 & p3
    Vec3 temp;
    Vec2 tempt;
    if (p1[1] > p2[1])
    {
        CopyVec(temp, p2, 3);
        CopyVec(p2, p1, 3);
        CopyVec(p1, temp, 3);
        if(tex != NULL){
            CopyVec(tempt, p2t, 2);
            CopyVec(p2t, p1t, 2);
            CopyVec(p1t, tempt, 2);}
    }

    if (p2[1] > p3[1])
    {
        CopyVec(temp, p2, 3);
        CopyVec(p2, p3, 3);
        CopyVec(p3, temp, 3);
        if(tex != NULL){
            CopyVec(tempt, p2t, 2);
            CopyVec(p2t, p3t, 2);
            CopyVec(p3t, tempt, 2);}
    }

    if (p1[1] > p2[1])
    {
        CopyVec(temp, p2, 3);
        CopyVec(p2, p1, 3);
        CopyVec(p1, temp, 3);
        if(tex != NULL){
            CopyVec(tempt, p2t, 2);
            CopyVec(p2t, p1t, 2);
            CopyVec(p1t, tempt, 2);}
    }

    // inverse slopes
    float dP1P2, dP1P3;

    // Computing inverse slopes
    if (p2[1] - p1[1] > 0)
        dP1P2 = (p2[0] - p1[0]) / (p2[1] - p1[1]);//cheque la pendiente 
    else
        dP1P2 = 0;

    if (p3[1] - p1[1] > 0)
        dP1P3 = (p3[0] - p1[0]) / (p3[1] - p1[1]);
    else
        dP1P3 = 0;

    
    Vertex v1, v2, v3;
    CopyVec(v1.coordinates, p1, 3);
    CopyVec(v2.coordinates, p2, 3);
    CopyVec(v3.coordinates, p3, 3);
    if(tex != NULL)
        CopyVec(v1.texCoordinates, p1t, 2);
        CopyVec(v2.texCoordinates, p2t, 2);
        CopyVec(v3.texCoordinates, p3t, 2);



    if(p1[1] != p2[1])
    {  
        if (dP1P2 > dP1P3)
        {
            ScanLineData data;
            int y;
            for ( y = (int)p1[1]; y <= (int)p3[1]; y++)
            {
                data.currentY = y;

                if (y < p2[1])
                {
                    ProcessScanLine_tex(&v1, &v3, &v1, &v2, color, SW, SH, sf, depthBuffer, &data, tex);
                }    
                else
                {
                    ProcessScanLine_tex(&v1, &v3, &v2, &v3, color, SW, SH, sf, depthBuffer, &data, tex);
                }   
            }
        }
        else
        {
            ScanLineData data;
            int y;
            for (y = (int)p1[1]; y <= (int)p3[1]; y++)
            {
                data.currentY = y;

                if (y < p2[1])
                {
                    ProcessScanLine_tex(&v1, &v2, &v1, &v3, color, SW, SH, sf, depthBuffer, &data, tex);
                }
                else
                {
                    ProcessScanLine_tex(&v2, &v3, &v1, &v3, color, SW, SH, sf, depthBuffer, &data, tex);
                }
            }
        }
    }
    else
    {
        if (p1[0] < p2[0])
        {
            ScanLineData data;
            int y;
            for ( y = (int)p1[1]; y <= (int)p3[1]; y++)
            {
                data.currentY = y;

                ProcessScanLine_tex(&v1, &v3, &v2, &v3, color, SW, SH, sf, depthBuffer, &data, tex);
            }
        }
        else
        {
            ScanLineData data;
            int y;
            for (y = (int)p1[1]; y <= (int)p3[1]; y++)
            {
                data.currentY = y;

                ProcessScanLine_tex(&v2, &v3, &v1, &v3, color, SW, SH, sf, depthBuffer, &data, tex);
            }
        }
    } 
}


void ProcessScanLine_esq(Vec3 va, Vec3 vb, Vec3 vc, Vec3 vd, Vec3 color, int SW, int SH, SDL_Surface* sf, int Y)
{
    // Thanks to current Y, we can compute the gradient to compute others values like
    // the starting X (sx) and ending X (ex) to draw between
    // if pa.Y == pb.Y or pc.Y == pd.Y, gradient is forced to 1

    float gradient1 = va[1] != vb[1] ? (Y - va[1]) / (vb[1] - va[1]) : 1;
    float gradient2 = vc[1] != vd[1] ? (Y - vc[1]) / (vd[1] - vc[1]) : 1;
            
    int sx = (int)Interpolate1(va[0], vb[0], gradient1);
    int ex = (int)Interpolate1(vc[0], vd[0], gradient2);

    Uint32 textureColor;

    textureColor = 0xfffffff;      //si no hay textura entonces pongo  255 

    Vec3 texColor;
    Uint32ToVec4(textureColor, texColor);
    VecByVec(texColor, color, texColor, 3);

    textureColor = Vec4ToUint32P(texColor[2], texColor[1], texColor[0]);

    DrawPoint_esq(sx, Y , SW, SH, sf, textureColor);
    DrawPoint_esq(ex, Y, SW, SH, sf, textureColor);  
}

void DrawTriangle_esq(Vec3 p1, Vec3 p2, Vec3 p3, Vec3 color, int SW, int SH, SDL_Surface* sf)
{
    // Sorting the points in order to always have this order on screen p1, p2 & p3
    // with p1 always up (thus having the Y the lowest possible to be near the top screen)
    // then p2 between p1 & p3
    Vec3 temp;
    Vec2 tempt;
    if (p1[1] > p2[1])
    {
        CopyVec(temp, p2, 3);
        CopyVec(p2, p1, 3);
        CopyVec(p1, temp, 3);
    }

    if (p2[1] > p3[1])
    {
        CopyVec(temp, p2, 3);
        CopyVec(p2, p3, 3);
        CopyVec(p3, temp, 3);
    }

    if (p1[1] > p2[1])
    {
        CopyVec(temp, p2, 3);
        CopyVec(p2, p1, 3);
        CopyVec(p1, temp, 3);
    }

    // inverse slopes
    float dP1P2, dP1P3;

    // Computing inverse slopes
    if (p2[1] - p1[1] > 0)
        dP1P2 = (p2[0] - p1[0]) / (p2[1] - p1[1]);//cheque la pendiente 
    else
        dP1P2 = 0;

    if (p3[1] - p1[1] > 0)
        dP1P3 = (p3[0] - p1[0]) / (p3[1] - p1[1]);
    else
        dP1P3 = 0;


    if(p1[1] != p2[1])
    {  
        if (dP1P2 > dP1P3)
        {            
            int y;
            for ( y = (int)p1[1]; y <= (int)p3[1]; y++)
            {
                
                if (y < p2[1])
                {
                    ProcessScanLine_esq(p1, p3, p1, p2, color, SW, SH, sf, y);
                }    
                else
                {
                    ProcessScanLine_esq(p1, p3, p2, p3, color, SW, SH, sf, y);
                }   
            }
        }
        else
        {            
            int y;
            for (y = (int)p1[1]; y <= (int)p3[1]; y++)
            {
                if (y < p2[1])
                {
                    ProcessScanLine_esq(p1, p2, p1, p3, color, SW, SH, sf, y);
                }
                else
                {
                    ProcessScanLine_esq(p2, p3, p1, p3, color, SW, SH, sf, y);
                }
            }
        }
    }
    else
    {
        if (p1[0] < p2[0])
        {
            int y;
            for ( y = (int)p1[1]; y <= (int)p3[1]; y++)
            {
                ProcessScanLine_esq(p1, p3, p2, p3, color, SW, SH, sf, y);
            }
        }
        else
        {
            int y;
            for (y = (int)p1[1]; y <= (int)p3[1]; y++)
            {
                ProcessScanLine_esq(p2, p3, p1, p3, color, SW, SH, sf, y);
            }
        }
    } 
}

void putpixel_esq(SDL_Surface *surface, int x, int y, Uint32 pixel, int swidth, int sheight)
{
    
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to set */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        *p = pixel;
        break;

    case 2:
        *(Uint16 *)p = pixel;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            p[0] = (pixel >> 16) & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = pixel & 0xff;
        } else {
            p[0] = pixel & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = (pixel >> 16) & 0xff;
        }
        break;

    case 4:
        *(Uint32 *)p = pixel;
        break;
    }
}

void DrawPoint_esq(int x, int y, int SW, int SH, SDL_Surface* sf, Uint32 color)
{
    if(x >= 0 && y >= 0 && x < SW && y <SH)
        putpixel_esq(sf, x, y, color, SW, SH);
}