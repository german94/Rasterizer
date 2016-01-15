#include "sdlHelper.h"
#include "mathHelper.h"
#include "modelLoader.h"


const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

SDL_Window* window = NULL;
SDL_Surface* screenSurface = NULL;

int mini(int a, int b){ if (a < b) {return a;} else {return b;} }

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

    Mat4 world, view, wvp, wv, proj;

    float angleOfView = 0.78; 
    float near = 0.1; 
    float far = 100; 

    Vec3 up = { 0, 1, 0 };
    Vec3 target = { 0, 0, -1 };
    Vec3 pos = { 0, 0, 8};

    CreateViewMatrix(view, target, pos, up);

    CreateProjectionMatrix(proj, near, far, angleOfView, SCREEN_WIDTH / SCREEN_HEIGHT);

    float a = 0;

    if(init())
    {
    	screenSurface = SDL_GetWindowSurface( window );

        Vec3DynamicArray Vertices, normals;
        UInt3DynamicArray faces;
        Vec2DynamicArray uvs;
        LoadModel("modelMonkey.obj", &Vertices, &uvs, &normals, &faces);

    	while(!quit)
    	{
            while(SDL_PollEvent(&e) != 0)
            {
            	if(e.type == SDL_QUIT)
            		quit = true;
            }

            SDL_FillRect( screenSurface, NULL, SDL_MapRGB( screenSurface->format, 0x00, 0x00, 0x00 ) );

            CreateRotationYMatrix(world, a);

            Mat4Product(world,view, wv);
            Mat4Product(wv, proj, wvp);
            a+=0.001f;
            RenderModel(&Vertices, &faces, &uvs, wvp, SCREEN_WIDTH, SCREEN_HEIGHT, screenSurface); 

            SDL_UpdateWindowSurface( window );
        }
    }

    SDL_DestroyWindow( window );

    SDL_Quit();

    return 0;
}