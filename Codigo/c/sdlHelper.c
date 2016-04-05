#include "sdlHelper.h"

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

float Interpolate2(float min, float max, float gradient)
{ 
	return min + (max - min) * Clamp(gradient);
}

float Interpolate1(float min, float max, float gradient)
{ 
    float mini = (gradient < 1) ? gradient : 1;
    float maxi = (0 > mini) ? 0 : mini;
    return min + (max - min) *maxi;
}
   

float ComputeNDotL(Vec3 centerPoint, Vec3 vnFace, Vec3 lightPos) 
{
  Vec3 lightDirection;

  Sub3(lightPos, centerPoint, lightDirection);

  Normalize3(vnFace);
  
  Normalize3(lightDirection);

  return Max(0, Dot3Prod(vnFace, lightDirection));
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

    float gradient1 = pa[1] != pb[1] ? (data->currentY - pa[1]) / (pb[1] - pa[1]) : 1;
    float gradient2 = pc[1] != pd[1] ? (data->currentY - pc[1]) / (pd[1] - pc[1]) : 1;
            
    int sx = (int)Interpolate2(pa[0], pb[0], gradient1); //Interpolate2 es el que llama a max y min; e Interpolate1 hace todo en una sola funcion
    int ex = (int)Interpolate2(pc[0], pd[0], gradient2);

    float z1 = Interpolate2(pa[2], pb[2], gradient1);
    float z2 = Interpolate2(pc[2], pd[2], gradient2);

    float snl = Interpolate2(data->ndotla, data->ndotlb, gradient1);
    float enl = Interpolate2(data->ndotlc, data->ndotld, gradient2);

    float su = Interpolate2(va->texCoordinates[0], vb->texCoordinates[0], gradient1);
    float eu = Interpolate2(vc->texCoordinates[0], vd->texCoordinates[0], gradient2);
    float sv = Interpolate2(va->texCoordinates[1], vb->texCoordinates[1], gradient1);
    float ev = Interpolate2(vc->texCoordinates[1], vd->texCoordinates[1], gradient2);

     int x;
    for (x = sx ; x < ex; x++)
    {
  		float gradient = (x - sx) / (float)(ex - sx);
        float z = Interpolate2(z1, z2, gradient);
        float ndotl = Interpolate2(snl, enl, gradient);

        float u = Interpolate2(su, eu, gradient);
        float v = Interpolate2(sv, ev, gradient);

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

    float gradient1 = pa[1] != pb[1] ? (data->currentY - pa[1]) / (pb[1] - pa[1]) : 1;
    float gradient2 = pc[1] != pd[1] ? (data->currentY - pc[1]) / (pd[1] - pc[1]) : 1;
            
    int sx = (int)Interpolate2(pa[0], pb[0], gradient1);
    int ex = (int)Interpolate2(pc[0], pd[0], gradient2);

    float z1 = Interpolate2(pa[2], pb[2], gradient1);
    float z2 = Interpolate2(pc[2], pd[2], gradient2);

    float su = Interpolate2(va->texCoordinates[0], vb->texCoordinates[0], gradient1);
    float eu = Interpolate2(vc->texCoordinates[0], vd->texCoordinates[0], gradient2);
    float sv = Interpolate2(va->texCoordinates[1], vb->texCoordinates[1], gradient1);
    float ev = Interpolate2(vc->texCoordinates[1], vd->texCoordinates[1], gradient2);

     int x;
    for (x = sx ; x < ex; x++)
    {
        float gradient = (x - sx) / (float)(ex - sx);
        float z = Interpolate2(z1, z2, gradient);

        float u = Interpolate2(su, eu, gradient);
        float v = Interpolate2(sv, ev, gradient);

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

void DrawBline_esq( int x0, int x1, int y0, int y1, SDL_Surface* sf, Vec3 vec_color, int SW, int SH)
{       
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;
    Uint32 Color = Vec4ToUint32P(vec_color[2], vec_color[1], vec_color[0]);

    while (true) {
 
        DrawPoint_esq(x0, y0, SW, SH, sf, Color);

        if ((x0 == x1) && (y0 == y1)) break;
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x0 += sx; }
        if (e2 < dx) { err += dx; y0 += sy; }
    }
}