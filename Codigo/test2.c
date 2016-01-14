#include "sdlHelper.h"
#include "mathHelper.h"
#include "vertexdata.h"



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

    Mat4 world, view, wvp, wv, proj, worldToCamera;

    float angleOfView = 0.78; 
    float near = 0.1; 
    float far = 100; 

    //deg to rad mpi/180

    Vec3 up = { 0, 1, 0 };
    Vec3 target = { 0, 0, -1 };
    Vec3 pos = { 0, 0, 3};

    CreateViewMatrix(view, target, pos, up);
   
    CreateProjectionMatrix(proj, near, far, angleOfView, 1.0f);

    float a = 0;

    if(init())
    {
    	screenSurface = SDL_GetWindowSurface( window );
		
    	int numVertices = 8; 
      float vertices[8][3] = {
        {-1.0, 1.0,  1.0},
        {1.0, 1.0,  1.0},
        {-1.0, -1.0,  1.0},
       
       {1.0,  -1.0,  1.0},
        {-1.0, 1.0, -1.0},
        {1.0,  1.0, -1.0},
        {1.0, -1.0, -1.0},
        {-1.0,  -1.0, -1.0}};

        int lineas[12][3] = {
        {0,1,2},
        {1,2,3},
        {1,3,6},
        {1,5,6},
        {0,1,4},
        {1,4,5},
        {2,3,7},
        {3,6,7},
        {0,2,7},
        {0,4,7},
        {4,5,6},
        {4,6,7}};

        int j;

        Mat4 t;
	
		t[0][0] = 1;	t[0][1] = 0;	t[0][2] = 0;	t[0][3] = 0;
		t[1][0] = 0;	t[1][1] = -1;	t[1][2] = 0;	t[1][3] = 0;
		t[2][0] = 0;	t[2][1] = 0;	t[2][2] = 1;	t[2][3] = 0;
		t[3][0] = 0;	t[3][1] = 0;	t[3][2] = 10;	t[3][3] = 1;


		while(!quit)
		{
			while(SDL_PollEvent(&e) != 0)
			{
				if(e.type == SDL_QUIT)
					quit = true;
			}

      SDL_FillRect( screenSurface, NULL, SDL_MapRGB( screenSurface->format, 0x00, 0x00, 0x00 ) );

      Mat4 world1;

      CreateRotationYMatrix(world1, a);

      Mat4Product(world1, t, world);

      Mat4Product(world,view, wv);
      Mat4Product(wv, proj, wvp);

      a+=0.001f;
      	int i, x0, x1, y0, y1;
		for(j = 0; j < 12; j++)
    	{

    		for(i = 0; i < 2; i++)
    		{
    			Vec4 res;
	            Vec3Mat4Product(vertices[lineas[j][i]], wvp, res);
	            x0 = (res[0]/res[3])*SCREEN_WIDTH*0.5 + SCREEN_WIDTH*0.5 ;
				y0 = (res[1]/res[3])*SCREEN_HEIGHT*0.5 + SCREEN_HEIGHT*0.5; 
	  		 	
				//SDL_FillRect( screenSurface, &srcrect, SDL_MapRGB( screenSurface->format, 0xFF, 0x00, 0x00 ) );
				Vec3Mat4Product(vertices[lineas[j][i+1]], wvp, res);
				x1 = (res[0]/res[3])*SCREEN_WIDTH*0.5 + SCREEN_WIDTH*0.5 ;
				y1 = (res[1]/res[3])*SCREEN_HEIGHT*0.5 + SCREEN_HEIGHT*0.5;
	
				DrawBline(  x0,  x1,  y0,  y1, screenSurface);
    		}
    		Vec4 res;

            Vec3Mat4Product(vertices[lineas[j][2]], wvp, res);
            x0 = (res[0]/res[3])*SCREEN_WIDTH*0.5 + SCREEN_WIDTH*0.5 ;
			y0 = (res[1]/res[3])*SCREEN_HEIGHT*0.5 + SCREEN_HEIGHT*0.5; 
  		 	
			Vec3Mat4Product(vertices[lineas[j][0]], wvp, res);
			x1 = (res[0]/res[3])*SCREEN_WIDTH*0.5 + SCREEN_WIDTH*0.5 ;
			y1 = (res[1]/res[3])*SCREEN_HEIGHT*0.5 + SCREEN_HEIGHT*0.5;

			DrawBline(  x0,  x1,  y0,  y1, screenSurface);
  		}
	  	
        SDL_UpdateWindowSurface( window );
    }
  }

    SDL_DestroyWindow( window );

    SDL_Quit();

    return 0;
}


