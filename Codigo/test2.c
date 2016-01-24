#include "sdlHelper.h"
#include "mathHelper.h"
#include "modelLoader.h"
#include "structHelper.h"
#include <float.h>
#include <stdlib.h>
 
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

SDL_Window* window = NULL;
SDL_Surface* screenSurface = NULL;
TTF_Font *font;

void initDepthBuffer(float *depthBuffer, int e)
{
	int i;
	for (i = 0; i < e; ++i)
		depthBuffer[i] = FLT_MAX;
}	 	
	
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

    TTF_Init();
    font = TTF_OpenFont("DroidSans.ttf", 12);

    return true;
}


int main( int argc, char* args[] )
{
    bool quit = false;
    SDL_Event e;

    Mat4 view, proj, worldToCamera;
    Mat4 scale;
    CreateScaleMatrix(scale, 2, 1, 1);

    float angleOfView = 0.78; 
    float near = 0.1; 
    float far = 100; 

    Vec3 up = { 0, 1, 0 };
    Vec3 target = { 0, 0, -1 };
    Vec3 pos = { 0, 0, 3};

    CreateViewMatrix(view, target, pos, up);
   
    CreateProjectionMatrix(proj, near, far, angleOfView, 1.0f);

    float a = 0;

    Uint32 startclock = 0;
    Uint32 deltaclock = 0;
    Uint32 currentFPS = 0;

    if(init())
    {
    	screenSurface = SDL_GetWindowSurface( window );

        Vec3DynamicArray Vertices;
        Vec2DynamicArray Uvs;
        Vec3DynamicArray Normals;
        UInt3DynamicArray Faces;

        LoadModel("model.obj", &Vertices,  &Uvs,  &Normals, &Faces);

        SDL_Surface* tex;
        if(Uvs.size != 0)
        	tex = SDL_LoadBMP("a.bmp");
        else
        	tex = NULL;

        Mat4 t;
	
        CreateTranslationMatrix(t, 0.0f, 0.0f, 20.0f);        //ESTO NO ANDA Y NO SE POR QUE!!

        float depthBuffer[SCREEN_WIDTH*SCREEN_HEIGHT];
      
		while(!quit)
		{
            startclock = SDL_GetTicks();

			while(SDL_PollEvent(&e) != 0)
			{
				if(e.type == SDL_QUIT)
					quit = true;
			}
			
			initDepthBuffer(depthBuffer, SCREEN_WIDTH*SCREEN_HEIGHT);

            SDL_FillRect( screenSurface, NULL, SDL_MapRGB( screenSurface->format, 0x00, 0x00, 0x00 ) );

            Mat4 world1, world, wv, wvp;

            CreateRotationYMatrix(world1, a);

            Mat4Product(world1, t, world);
            Mat4Product(world, scale, world1);

            Mat4Product(world1,view, wv);
            
            Mat4Product(wv, proj, wvp);

            a+=0.01f;
      	
            RenderFilledModel(&Vertices,  &Uvs,  &Normals, &Faces, wvp, SCREEN_WIDTH, SCREEN_HEIGHT, screenSurface, depthBuffer, world, tex); 

            deltaclock = SDL_GetTicks() - startclock;
            startclock = SDL_GetTicks();
                    
            if ( deltaclock != 0 )
                currentFPS = 1000 / deltaclock;

            char buf[9];
            strcpy(buf, "FPS: ");
            snprintf(&buf[5], 4, "%d", currentFPS);

            SDL_Color clrFg = {255,0,0,0};
            RenderText(clrFg, font, buf, screenSurface);
            

        	SDL_UpdateWindowSurface( window );
    	}
  	}

    TTF_CloseFont(font);
    SDL_DestroyWindow( window );

    SDL_Quit();

    return 0;
}