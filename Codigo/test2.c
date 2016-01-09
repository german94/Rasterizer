#include "sdlHelper.h"
#include "mathHelper.h"
#include <stdio.h>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

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

//PASOS:
//1. Crear la matriz view (v) y projection (p)
//2. Crear los vertices del modelo 3D
//3. Crear la matriz de world (w)
//4. En el loop del render, multiplicar cada vertice del modelo por wvp. Posteriormente dividir por z (o -z) las coordenadas x e y
//5. Ahora falta mapear las coordenadas x e y de cada punto, a pixeles. Para esto: x = (x + width / 2) / width y lo mismo para y con height.
//6. El ultimo paso para convertir a pixeles es hacer el redondeo de las coordenadas x e y multiplicadas por width y height. Ademas hay que
//   invertir la coordenada y: y = floor((1- y) * height), x = floor(x * width) 

int main( int argc, char* args[] )
{
    bool quit = false;
    SDL_Event e;

    Mat4 world, view, proj, wvp;
    Vec3 up = { 0, 1, 0 };
    Vec3 target = { 0, 0, -1 };
    Vec3 pos = { 0, 0, 3 };
    CreateViewMatrix(view, target, pos, up);
    CreateProjectionMatrix(proj, 0.001f, 1.0f, 0.78f, SCREEN_WIDTH / SCREEN_HEIGHT);
    Vec3 posCube = { 3, 0, 0 };
    CreateTranslationMatrix(world, posCube);
    Mat4Product(world, view, view);
    Mat4Product(view, proj, wvp);
    //DisplayMatrix(world);
    //DisplayMatrix(view);
    //DisplayMatrix(proj);

    float a = 0;

    if(init())
    {
    	screenSurface = SDL_GetWindowSurface( window );
		
        float cubo[8][4] = {
           {-1, 1, 1, 1},
           {1, 1, 1, 1},
           {-1, -1, 1, 1},
           {-1, -1, -1, 1},
           {-1, 1, -1, 1},
           {1, 1, -1, 1},
           {1, -1, 1, 1},
           {1, -1, -1, 1}
        };

		while(!quit)
		{
			while(SDL_PollEvent(&e) != 0)
			{
				if(e.type == SDL_QUIT)
					quit = true;
			}

            SDL_FillRect(screenSurface, NULL, SDL_MapRGB( screenSurface->format, 0x00, 0x00, 0x00 ));

            int j;
            for(j = 0; j < 8; j++)
            {
                Vec4 res;
                Vec4Mat4Product(cubo[j], wvp, res);
                float x = ((res[0] / (-cubo[j][2])) + SCREEN_WIDTH / 2) / SCREEN_WIDTH; 
                float y = ((res[1] / (-cubo[j][2])) + SCREEN_HEIGHT / 2) / SCREEN_HEIGHT;
                
                putpixel(screenSurface, floor(x * SCREEN_WIDTH), floor((1 - y) * SCREEN_HEIGHT), 0xFF0000);
            }

	        SDL_UpdateWindowSurface( window );
    	}
    }

    SDL_DestroyWindow( window );

    SDL_Quit();

    return 0;
}
