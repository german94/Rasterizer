#include "sdlHelper.h"
#include <stdio.h>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

SDL_Window* window = NULL;
SDL_Surface* screenSurface = NULL;

bool init()
{
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
        return false;
    }	

    window = SDL_CreateWindow( "TP Final", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
    if( window == NULL )
    {
        printf( "Error al crear la ventana: %s\n", SDL_GetError() );
        return false;
    }

    return true;
}


int main( int argc, char* args[] )
{
    bool quit = false;
    SDL_Event e;

    if(init())
    {
    	screenSurface = SDL_GetWindowSurface( window );
		SDL_FillRect( screenSurface, NULL, SDL_MapRGB( screenSurface->format, 0xFF, 0xFF, 0xFF ) );
		int i;
		for(i = 0; i < 50; i++)
			putpixel(screenSurface, i, 20, 0xFFFF0000);
	
		while(!quit)
		{
			while(SDL_PollEvent(&e) != 0)
			{
				if(e.type == SDL_QUIT)
					quit = true;
			}
	        
	        SDL_UpdateWindowSurface( window );
    	}
    }

    SDL_DestroyWindow( window );

    SDL_Quit();

    return 0;
}