#include "sdlHelper.h"
//#include "mathHelper.h"

void putpixel(SDL_Surface *surface, int x, int y, float z, Vec3 color, int swidth, int sheight, float* depthBuffer, float ndotl)
{
	
    Uint32 color1 = color[0]*ndotl*255.0f;
    Uint32 color2 = color[1]*ndotl*255.0f;
    Uint32 color3 = color[2]*ndotl*255.0f;

	int index = x + y*swidth;
	if(depthBuffer[index] > z)
	{
  		depthBuffer[index] = z;
	    int bpp = surface->format->BytesPerPixel;
	    /* Here p is the address to the pixel we want to set */
	    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

	    switch(bpp) {
	    case 1:
	        *p = color1;
	        break;

	    case 2:
	        *(Uint16 *)p = color1;
	        break;

	    case 3:
	        if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
	           // p[0] = (pixel >> 16) & 0xff;
	           // p[1] = (pixel >> 8) & 0xff;
	           // p[2] = pixel & 0xff;
                p[0] =  color1 & 0xff;
                p[1] =  color2 & 0xff;
                p[2] =  color3 & 0xff;
	        } else {
	           // p[0] = pixel & 0xff;
	          //  p[1] = (pixel >> 8) & 0xff;
	          //  p[2] = (pixel >> 16) & 0xff;
                p[0] = color3 & 0xff;
                p[1] = color2 & 0xff;
                p[2] = color1 & 0xff;
	        }
	        break;

	    case 4:
	        *(Uint32 *)p = color1;
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

void DrawBline( int x0, int x1, int y0, int y1, SDL_Surface* screenSurface)
{       
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;
    SDL_Rect srcrect;
    srcrect.h = 1;
    srcrect.w = 1;
    while (true) {
        //DrawPoint(new Vector2(x0, y0));
        srcrect.x = x0;
        srcrect.y = y0;
        SDL_FillRect( screenSurface, &srcrect, SDL_MapRGB( screenSurface->format, 0xFF, 0x00, 0x00 ) );

        if ((x0 == x1) && (y0 == y1)) break;
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x0 += sx; }
        if (e2 < dx) { err += dx; y0 += sy; }
    }
}


void DrawPoint(int x, int y, float z, float* depthBuffer, int SW, int SH, SDL_Surface* sf, Vec3 color, float ndotl)
{
    if(x >= 0 && y >= 0 && x < SW && y <SH)
        putpixel(sf, x, y, z, color, SW, SH, depthBuffer, ndotl);
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


float Interpolate(float min, float max, float gradient)
{ 
	return min + (max - min) * Clamp(gradient);
}


float ComputeNDotL(Vec3 centerPoint, Vec3 vnFace, Vec3 lightPos) 
{
  Vec3 lightDirection;

  Sub3(lightPos, centerPoint, lightDirection);

  Normalize3(vnFace);
  
  Normalize3(lightDirection);

  return Max(0, Dot3Prod(vnFace, lightDirection));
}


void ProcessScanLine(int y, Vec3 pa, Vec3 pb, Vec3 pc, Vec3 pd, Vec3 color, int SW, int SH, SDL_Surface* sf, float* depthBuffer, float ndotla, float ndotlb, float ndotlc, float ndotld)
{
    // Thanks to current Y, we can compute the gradient to compute others values like
    // the starting X (sx) and ending X (ex) to draw between
    // if pa.Y == pb.Y or pc.Y == pd.Y, gradient is forced to 1
    float gradient1 = pa[1] != pb[1] ? (y - pa[1]) / (pb[1] - pa[1]) : 1;
    float gradient2 = pc[1] != pd[1] ? (y - pc[1]) / (pd[1] - pc[1]) : 1;
            
    int sx = (int)Interpolate(pa[0], pb[0], gradient1);
    int ex = (int)Interpolate(pc[0], pd[0], gradient2);

    // drawing a line from left (sx) to right (ex) 
    float z1 = Interpolate(pa[2], pb[2], gradient1);
    float z2 = Interpolate(pc[2], pd[2], gradient2);

    float snl = Interpolate(ndotla, ndotlb, gradient1);
    float enl = Interpolate(ndotlc, ndotld, gradient2);

    int x;
    for (x = sx ; x < ex; x++)
    {
  		float gradient = (x - sx) / (float)(ex - sx);
        float z = Interpolate(z1, z2, gradient);
        float ndotl = Interpolate(snl, enl, gradient);

        DrawPoint(x, y, z, depthBuffer, SW, SH, sf, color, ndotl);
    }
}

void DrawTriangle(Vec3 p1, Vec3 p2, Vec3 p3, Vec3 p1n, Vec3 p2n, Vec3 p3n, Vec3 p1w, Vec3 p2w, Vec3 p3w, Vec3 color, int SW, int SH, SDL_Surface* sf, float* depthBuffer)
{
    // Sorting the points in order to always have this order on screen p1, p2 & p3
    // with p1 always up (thus having the Y the lowest possible to be near the top screen)
    // then p2 between p1 & p3
    Vec3 temp, tempn, tempw;
    if (p1[1] > p2[1])
    {
     	CopyVec3(temp, p2);
     	CopyVec3(p2, p1);
     	CopyVec3(p1, temp);
        CopyVec3(tempn, p2n);
        CopyVec3(p2n, p1n);
        CopyVec3(p1n, tempn);
        CopyVec3(tempw, p2w);
        CopyVec3(p2w, p1w);
        CopyVec3(p1w, tempw);
    }

    if (p2[1] > p3[1])
    {
     	CopyVec3(temp, p2);
     	CopyVec3(p2, p3);
     	CopyVec3(p3, temp);
        CopyVec3(tempn, p2n);
        CopyVec3(p2n, p3n);
        CopyVec3(p3n, tempn);
        CopyVec3(tempw, p2w);
        CopyVec3(p2w, p3w);
        CopyVec3(p3w, tempw);
    }

    if (p1[1] > p2[1])
    {
     	CopyVec3(temp, p2);
     	CopyVec3(p2, p1);
     	CopyVec3(p1, temp);
        CopyVec3(tempn, p2n);
        CopyVec3(p2n, p1n);
        CopyVec3(p1n, tempn);
        CopyVec3(tempw, p2w);
        CopyVec3(p2w, p1w);
        CopyVec3(p1w, tempw);
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

    // First case p2 right; second p2 left

    if(p1[1] != p2[1])
    {  
    	if (dP1P2 > dP1P3)
    	{
        	int y;
        	for ( y = (int)p1[1]; y <= (int)p3[1]; y++)
        	{
        		if (y < p2[1])
                {
                    float ndotla = nl1;
                    float ndotlb = nl3;
                    float ndotlc = nl1;
                    float ndotld = nl2;
                    ProcessScanLine(y, p1, p3, p1, p2, color, SW, SH, sf, depthBuffer, ndotla, ndotlb, ndotlc, ndotld);
                }    
            	else
            	{
                    float ndotla = nl1;
                    float ndotlb = nl3;
                    float ndotlc = nl2;
                    float ndotld = nl3;
                    ProcessScanLine(y, p1, p3, p2, p3, color, SW, SH, sf, depthBuffer, ndotla, ndotlb, ndotlc, ndotld);
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
                    float ndotla = nl1;
                    float ndotlb = nl2;
                    float ndotlc = nl1;
                    float ndotld = nl3;
                    ProcessScanLine(y, p1, p2, p1, p3, color, SW, SH, sf, depthBuffer, ndotla, ndotlb, ndotlc, ndotld);
                }
            	else
        		{
                    float ndotla = nl2;
                    float ndotlb = nl3;
                    float ndotlc = nl1;
                    float ndotld = nl3;
                    ProcessScanLine(y, p2, p3, p1, p3, color, SW, SH, sf, depthBuffer, ndotla, ndotlb, ndotlc, ndotld);
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
        		if (y < p2[1])
            		{
                        float ndotla = nl1;
                        float ndotlb = nl3;
                        float ndotlc = nl1;
                        float ndotld = nl2;
                        ProcessScanLine(y, p1, p3, p1, p2, color, SW, SH, sf, depthBuffer, ndotla, ndotlb, ndotlc, ndotld);
                    }
        		else
            		{
                        float ndotla = nl1;
                        float ndotlb = nl3;
                        float ndotlc = nl2;
                        float ndotld = nl3;
                        ProcessScanLine(y, p1, p3, p2, p3, color, SW, SH, sf, depthBuffer, ndotla, ndotlb, ndotlc, ndotld);
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
                        float ndotla = nl1;
                        float ndotlb = nl2;
                        float ndotlc = nl1;
                        float ndotld = nl3;
                        ProcessScanLine(y, p1, p2, p1, p3, color, SW, SH, sf, depthBuffer, ndotla, ndotlb, ndotlc, ndotld);
                    }
        		else
              		{
                        float ndotla = nl2;
                        float ndotlb = nl3;
                        float ndotlc = nl1;
                        float ndotld = nl3;
                        ProcessScanLine(y, p2, p3, p1, p3, color, SW, SH, sf, depthBuffer, ndotla, ndotlb, ndotlc, ndotld);
                    }
        	}
        }
    } 
}