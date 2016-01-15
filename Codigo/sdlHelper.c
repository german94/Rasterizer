#include "sdlHelper.h"

void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
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

void Translation(int (*Msrc)[3], int (*Mdst)[3], int cant_pixel, int *trans)
{
	int i;
 	for(i=0; i< cant_pixel; i++)
	   	{
	   		Mdst[i][0] = Msrc[i][0] + trans[0];
	   		Mdst[i][1] = Msrc[i][1] + trans[1];
	   		Mdst[i][2] = Msrc[i][2] + trans[2];
	    }
}

void RotationX(int (*Mdst)[3], int (*Msrc)[3], int angulo, int cant_pixel)  
{ 		
	int i;
	for(i=0; i< cant_pixel; i++)
	{
	   	Mdst[i][0] = Msrc[i][0];
	   	Mdst[i][1] = Msrc[i][1]*cos(angulo) - sin(angulo)*Msrc[i][2];
	   	Mdst[i][2] = Msrc[i][1]*sin(angulo) + cos(angulo)*Msrc[i][2];
	}
}

void Coordto2d(int (*Msrc)[3], int (*Mdst)[2], int cant_pixel)
{	   	
	int j;	
	for(j = 0; j < cant_pixel; j++)
	{
		Mdst[j][0] =(Msrc[j][0]-Msrc[j][1])/sqrt(2);
		Mdst[j][1]=(Msrc[j][0]+2*Msrc[j][2]+Msrc[j][1])/sqrt(6);
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

void formar_cubo(int (*cubo)[3], int distancia)
{
   	cubo[0][0] = 0;
   	cubo[0][1] = 0;
   	cubo[0][2] = 0;
   	cubo[1][0] = distancia;
   	cubo[1][1] = 0;
   	cubo[1][2] = 0;
   	cubo[2][0] = distancia;
   	cubo[2][1] = 0;
   	cubo[2][2] = distancia;
   	cubo[3][0] = 0;
   	cubo[3][1] = 0;
   	cubo[3][2] = distancia;

   	int indice, i, j;
   	for( indice = 4; indice < 8; indice++ )
   	{
   		cubo[indice][0] = cubo[indice -4][0];
   		cubo[indice][1] = cubo[indice -4][1] + distancia;
   		cubo[indice][2] = cubo[indice - 4][2];
   	}
   	for (i = 0; i < 4; i++)
   	{
   		for( j = 0; j < distancia - 1; j++)
   		{
   			switch(i) {
   				case 0 :
   					cubo[indice][0] = j+1;
					cubo[indice][1] = 0;
					cubo[indice][2] = 0;
					cubo[indice + distancia -1][0] = j+1;
					cubo[indice + distancia -1][1] = distancia;
					cubo[indice + distancia -1][2] = 0; 
					indice++;
					break;
				case 1 :
   					cubo[indice][0] = distancia;
					cubo[indice][1] = 0;
					cubo[indice][2] = j+1;
					cubo[indice + distancia -1][0] = distancia;
					cubo[indice + distancia -1][1] = distancia;
					cubo[indice + distancia -1][2] = j+1;
					indice++; 
					break;
				case 2 :
   					cubo[indice][0] = j+1;
					cubo[indice][1] = 0;
					cubo[indice][2] = distancia;
					cubo[indice + distancia -1][0] = j+1;
					cubo[indice + distancia -1][1] = distancia;
					cubo[indice + distancia -1][2] = distancia; 
					indice++;
					break;				
				case 3 :
   					cubo[indice][0] = 0;
					cubo[indice][1] = 0;
					cubo[indice][2] = j+1;
					cubo[indice + distancia -1][0] = 0;
					cubo[indice + distancia -1][1] = distancia;
					cubo[indice + distancia -1][2] = j+1; 
					indice++;
					break;	
			}
   		}	
   		indice += distancia - 1;
   	}	

   	for (i = 0; i < 2; i++)
   	{
   		for( j = 0; j < distancia - 1; j++)
   		{
   			switch(i) {
   				case 0 :
   					cubo[indice][0] = 0;
					cubo[indice][1] = j+1;
					cubo[indice][2] = 0;
					cubo[indice + distancia -1][0] = 0;
					cubo[indice + distancia -1][1] = j+1;
					cubo[indice + distancia -1][2] = distancia; 
					indice++;
					break;
				case 1 :
   					cubo[indice][0] = distancia;
					cubo[indice][1] = j+1;
					cubo[indice][2] = 0;
					cubo[indice + distancia -1][0] = distancia;
					cubo[indice + distancia -1][1] = j+1;
					cubo[indice + distancia -1][2] = distancia; 
					indice++;
					break;	
			}
   		}
   		indice += distancia - 1;	
   	}
}