#include "sdlHelper.h"
#include "mathHelper.h"
#include "modelLoader.h"
#include "structHelper.h"
#include <float.h>
 
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

SDL_Window* window = NULL;
SDL_Surface* screenSurface = NULL;

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

    return true;
}


int main( int argc, char* args[] )
{
	

    bool quit = false;
    SDL_Event e;

    Mat4 view, proj, worldToCamera;

    float angleOfView = 0.78; 
    float near = 0.1; 
    float far = 100; 

    Vec3 up = { 0, 1, 0 };
    Vec3 target = { 0, 0, -1 };
    Vec3 pos = { 0, 0, 3};

    CreateViewMatrix(view, target, pos, up);
   
    CreateProjectionMatrix(proj, near, far, angleOfView, 1.0f);

    float a = 0;

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
	
		t[0][0] = 1;	t[0][1] = 0;	t[0][2] = 0;	t[0][3] = 0;
		t[1][0] = 0;	t[1][1] = -1;	t[1][2] = 0;	t[1][3] = 0;
		t[2][0] = 0;	t[2][1] = 0;	t[2][2] = -1;	t[2][3] = 0;
		t[3][0] = 0;	t[3][1] = 0;	t[3][2] = 20;	t[3][3] = 1;

        float depthBuffer[SCREEN_WIDTH*SCREEN_HEIGHT];
      
		while(!quit)
		{
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

            Mat4Product(world,view, wv);
            
            Mat4Product(wv, proj, wvp);

            a+=0.01f;
      	
            RenderFilledModel(&Vertices,  &Uvs,  &Normals, &Faces, wvp, SCREEN_WIDTH, SCREEN_HEIGHT, screenSurface, depthBuffer, world, tex); 

        	SDL_UpdateWindowSurface( window );
    	}
  	}

    SDL_DestroyWindow( window );

    SDL_Quit();

    return 0;
}