#include "sdlHelper.h"
//#include "mathHelper.h"

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


float Interpolate(float min, float max, float gradient)
{ 
	return min + (max - min) * Clamp(gradient);
}

void ProcessScanLine(int y, Vec3 pa, Vec3 pb, Vec3 pc, Vec3 pd, Uint32 color, int SW, int SH, SDL_Surface* sf, float* depthBuffer)
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

    int x;
    for (x = sx ; x < ex; x++)
    {
  		float gradient = (x - sx) / (float)(ex - sx);
        float z = Interpolate(z1, z2, gradient);
        DrawPoint(x, y, z, depthBuffer, SW, SH, sf, color);
    }
}


void DrawTriangle(Vec3 p1, Vec3 p2, Vec3 p3, Uint32 color, int SW, int SH, SDL_Surface* sf, float* depthBuffer)
{
  
    // Sorting the points in order to always have this order on screen p1, p2 & p3
    // with p1 always up (thus having the Y the lowest possible to be near the top screen)
    // then p2 between p1 & p3
    Vec3 temp;
    if (p1[1] > p2[1])
    {
     	CopyVec3(temp, p2);
     	CopyVec3(p2, p1);
     	CopyVec3(p1, temp);
    }

    if (p2[1] > p3[1])
    {
     	CopyVec3(temp, p2);
     	CopyVec3(p2, p3);
     	CopyVec3(p3, temp);
    }

    if (p1[1] > p2[1])
    {
     	CopyVec3(temp, p2);
     	CopyVec3(p2, p1);
     	CopyVec3(p1, temp);
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

    // First case p2 right; second p2 left

    if(p1[1] != p2[1])
    {  
    	if (dP1P2 > dP1P3)
    	{
        	int y;
        	for ( y = (int)p1[1]; y <= (int)p3[1]; y++)
        	{
        		if (y < p2[1])
            		ProcessScanLine(y, p1, p3, p1, p2, color, SW, SH, sf, depthBuffer);
            	else
            		ProcessScanLine(y, p1, p3, p2, p3, color, SW, SH, sf, depthBuffer);
        	}
		}
    	else
    	{
        	int y;
        	for (y = (int)p1[1]; y <= (int)p3[1]; y++)
        	{
            	if (y < p2[1])
            		ProcessScanLine(y, p1, p2, p1, p3, color, SW, SH, sf, depthBuffer);
            	else
            		ProcessScanLine(y, p2, p3, p1, p3, color, SW, SH, sf, depthBuffer);
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
            		ProcessScanLine(y, p1, p3, p1, p2, color, SW, SH, sf, depthBuffer);
        		else
            		ProcessScanLine(y, p1, p3, p2, p3, color, SW, SH, sf, depthBuffer);
        	}
        }
    	else
    	{
        	int y;
        	for (y = (int)p1[1]; y <= (int)p3[1]; y++)
        	{
        		if (y < p2[1])
            		ProcessScanLine(y, p1, p2, p1, p3, color, SW, SH, sf, depthBuffer);
        		else
              		ProcessScanLine(y, p2, p3, p1, p3, color, SW, SH, sf, depthBuffer);
        	}
        }
    } 
}