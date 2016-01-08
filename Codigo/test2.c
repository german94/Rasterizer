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


int main( int argc, char* args[] )
{
    bool quit = false;
    SDL_Event e;

    Mat4 world, view, proj, wvp, wv;
    Vec3 up = { 0, 1, 0 };
    Vec3 target = { 0.4f, 0.3f, 0.3f };
    Vec3 pos = { 0, 0, -4 };
    CreateViewMatrix(view, target, pos, up);
    CreateProjectionMatrix(proj, 0.001f, 1.0f, 0.25f * M_PI, SCREEN_WIDTH / SCREEN_HEIGHT);
    //DisplayMatrix(world);
    //DisplayMatrix(view);
    //DisplayMatrix(proj);

    float a = 0;

    if(init())
    {
    	screenSurface = SDL_GetWindowSurface( window );
		

        float cubo[24][4] = {
           {-1.0, -1.0,  1.0, 1.0f},
   {1.0, -1.0,  1.0, 1.0f},
   {1.0,  1.0,  1.0, 1.0f},
  {-1.0,  1.0,  1.0, 1.0f},
  
  // Back face
  {-1.0, -1.0, -1.0, 1.0f},
  {-1.0,  1.0, -1.0, 1.0f},
  {1.0,  1.0, -1.0, 1.0f},
   {1.0, -1.0, -1.0, 1.0f},
  
  // Top face
  {-1.0,  1.0, -1.0, 1.0f},
  {-1.0,  1.0,  1.0, 1.0f},
   {1.0,  1.0,  1.0, 1.0f},
   {1.0,  1.0, -1.0, 1.0f},
  
  // Bottom face
  {-1.0, -1.0, -1.0, 1.0f},
   {1.0, -1.0, -1.0, 1.0f},
   {1.0, -1.0,  1.0, 1.0f},
  {-1.0, -1.0,  1.0, 1.0f},
  
  // Right face
   {1.0, -1.0, -1.0, 1.0f},
   {1.0,  1.0, -1.0, 1.0f},
   {1.0,  1.0,  1.0, 1.0f},
   {1.0, -1.0,  1.0, 1.0f},
  
  // Left face
  {-1.0, -1.0, -1.0, 1.0f},
  {-1.0, -1.0,  1.0, 1.0f},
  {-1.0,  1.0,  1.0, 1.0f},
  {-1.0,  1.0, -1.0, 1.0f}
        };

        float cubo2D[24][4];
        int j;
        

		while(!quit)
		{
			while(SDL_PollEvent(&e) != 0)
			{
				if(e.type == SDL_QUIT)
					quit = true;
			}

            SDL_FillRect( screenSurface, NULL, SDL_MapRGB( screenSurface->format, 0x00, 0x00, 0x00 ) );

            CreateRotationYMatrix(world, a);
            Mat4Product(world, view, wv);
            Mat4Product(wv, proj, wvp);

            a+=0.001f;

            for(j = 0; j < 24; j++)
            {
                Vec4Mat4Product(cubo[j], wvp, cubo2D[j]);
                cubo2D[j][0] = (cubo2D[j][0] / -cubo[j][3] + SCREEN_WIDTH / 2); 
                cubo2D[j][1] = (cubo2D[j][1] / -cubo[j][3] + SCREEN_HEIGHT / 2);
                
                putpixel(screenSurface, cubo2D[j][0], cubo2D[j][1], 0xFF0000);
                if(a == 0.001f)
                printf("%f %f %f %f\n", cubo2D[j][0], cubo2D[j][1], cubo2D[j][2], cubo2D[j][3]);
            }

	        
	        SDL_UpdateWindowSurface( window );
    	}
    }

    SDL_DestroyWindow( window );

    SDL_Quit();

    return 0;
}