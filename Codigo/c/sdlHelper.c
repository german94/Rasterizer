#include "sdlHelper.h"

void putpixel(int x, int y, float z, float* depthBuffer, SDL_Surface* surface, Uint32 pixel)
{
	
	int index = x + y*surface->w;
	if(x >= 0 && y >= 0 && x < surface->w && y < surface->h && depthBuffer[index] > z)
	{
  		depthBuffer[index] = z;
	    int bpp = surface->format->BytesPerPixel;

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
        return 0;
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
    outColor[0] = ((inColor >> 16) & 255) / 255.0f;// b
    outColor[1] = ((inColor >> 8) & 255) / 255.0f;// g
    outColor[2] = (inColor & 255) / 255.0f; // r 
}//devuelvo (b,g,r)


Uint32 Vec4ToUint32P(float _r, float _g, float _b)
{
    Uint32 r = (Uint32)(_r * 255.0f) & 255;
    Uint32 g = (Uint32)(_g * 255.0f) & 255;
    Uint32 b = (Uint32)(_b * 255.0f) & 255;

    Uint32 value = r;
    value |= g << 8;
    value |= b << 16;

    return (Uint32)value;
}

void putpixel_m1(int x, int y, SDL_Surface* surface, Uint32 pixel)
{
    if(x >= 0 && y >= 0 && x < surface->w && y < surface->h)
    {
	    int bpp = surface->format->BytesPerPixel;
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