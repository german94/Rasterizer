#include "sdlHelper.h"
#include <stdio.h>
#include <math.h>


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
		//si el segundo param es null pinta toda la pantalla tmb esta Uint32 SDL_MapRGBA(const SDL_PixelFormat* format,
      
		int i, indice, j;

		//para formar el cubo
		int distancia = 100;
		int cant_pixel = 8 + (distancia -1)*12;
	   	int cubo[cant_pixel][3];
	   	formar_cubo(cubo, distancia);
	   	//////////
	   
	   	int trans[3]; //posicion a trasladar

	   	/*
		int i;
		for(i = 0; i < 50; i++)
			putpixel(screenSurface, i, 20, 0x2E2EFE);
	//void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
		*/

	   	int angulo = 0;
	   	
	   	int cubo2[cant_pixel][2];//son las coordenadas finales en 2d

		int Mdst[cant_pixel][3]; //aca se guardan todos los cambios a la matriz cubo original

		while(!quit)
		{
			while(SDL_PollEvent(&e) != 0)
			{
				if(e.type == SDL_QUIT)
					quit = true;
			}
	        
			SDL_FillRect( screenSurface, NULL, SDL_MapRGB( screenSurface->format, 0xFF, 0xFF, 0xFF ) );//limpio la pantalla
	   	
			RotationX(Mdst, cubo, angulo, cant_pixel);

			angulo++; //solo para que despues se mueva  
			angulo = angulo%360;

			trans[0] = rand() % 200; trans[1] = rand() % 200; trans[2] = rand() % 200;
			Translation(Mdst, Mdst, cant_pixel, trans);

	   		//perspectiva isometrica se llama este pasaje/*
	   		Coordto2d(Mdst, cubo2, cant_pixel);

	        for(j = 0; j < 8 + (distancia - 1)*12; j++)//como la pantalla indexa desde la parte de arriba necesito mover las coordenadas finales
	   		{//para que muestre bien hay que solucionar esto
	    		cubo2[j][0]+=100;
	    		cubo2[j][1]+=100;
	    	}

			for(i = 0; i < cant_pixel; i++)
	    	{	//dibujo
	    		SDL_Rect srcrect;
    			srcrect.x = cubo2[i][0];
				srcrect.y = cubo2[i][1];
				srcrect.w = 4;//un determinado ancho y alto
				srcrect.h = 4;
	    		SDL_FillRect( screenSurface, &srcrect, SDL_MapRGB( screenSurface->format, 0xFF, 0x00, 0x00 ) );
	   		}	
	        
	        SDL_UpdateWindowSurface( window );
	        sleep(1);
    	}
    }

    SDL_DestroyWindow( window );

    SDL_Quit();

    return 0;
}