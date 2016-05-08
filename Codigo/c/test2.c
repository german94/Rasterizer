#include <float.h>
#include <stdlib.h>
#include <time.h>  
#include "auxiliares.h"
 
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 800;

extern void initDepthBufferASM(float *buffer, float *maxs, int tam);

SDL_Window* window;
SDL_Surface* screenSurface;
TTF_Font *font;
bool quit;
SDL_Event e;
bool rotX;
bool rotY;
bool rotZ;
bool m_esq;
bool m_tex;
bool m_tex_norm;
float sx, sy, sz;
float depthBuffer[800 * 800];       //no deja usar las consts por declararlo globalmente
Mat4 scale;
float rotSpeed;
bool showFPS;
bool showInfo;
Vec3 lightPos;
Vec3 color = {1.0, 1.0, 1.0};
Mat4 t;

extern bool interpolate1ON;

int main(int argc, char *argv[])
{

    Mat4 view, proj, worldToCamera;

    float angleOfView = 0.78; 
    float near = 0.1; 
    float far = 100; 

    Vec4 vec_max = {FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX};
    Vec3 up = { 0, 1, 0 };
    Vec3 target = { 0, 0, -1 };
    Vec3 pos = { 0, 0, 3};
    lightPos[0] = 0; lightPos[1] = 0; lightPos[2] = 20;

    CreateViewMatrix(view, target, pos, up);
    Traspose(view);
   
    CreateProjectionMatrix(proj, near, far, angleOfView);
    Traspose(proj);

    float a = 0;

    Uint32 startclock = 0;
    Uint32 deltaclock = 0;
    Uint32 currentFPS = 0;

    Vec4DynamicArray Vertices;
    Vec2DynamicArray Uvs;
    Vec4DynamicArray Normals;
    UInt3DynamicArray Faces;

    if(init())
    {
        CreateScaleMatrix(scale, sx, sy, sz);
        Traspose(scale);

    	screenSurface = SDL_GetWindowSurface( window );

        Vec2_int max_distancias; //{indice x_max, indice y_max}

        Model data;
        data.vertices = &Vertices;
        data.uvs = &Uvs;
        data.normals = &Normals;
        data.faces = &Faces;
        data.color = color;

        data.max_dis = max_distancias;  


        LoadModel(argv[1], &data);
          
        SDL_Surface* tex;
        if(Uvs.size != 0)
        {
            if(argc == 3)
                tex = SDL_LoadBMP(argv[2]);
            else
            {
                printf("Falta textura.\n");
                quit = true;
            }
        }
        else
            tex = NULL;


        CreateTranslationMatrix(t, 0.0f, 0.0f, 20.0f);
        
        Mat4 worldt;

        //Uint32 startApp = SDL_GetTicks();
		while(!quit)
		{
            startclock = SDL_GetTicks();

			EventDetection();
						 			 
            //initDepthBufferASM(depthBuffer, vec_max, SCREEN_WIDTH*SCREEN_HEIGHT);
            initDepthBuffer();

            SDL_FillRect( screenSurface, NULL, SDL_MapRGB( screenSurface->format, 0x00, 0x00, 0x00 ) );

            Mat4 mRotX, mRotY, mRotZ;
            Identity(mRotX);
            Identity(mRotY);
            Identity(mRotZ);
            Mat4 world1, world, wv, wvp;
            Identity(world);
            data.wvp = wvp;
            data.world = worldt;

            if(rotY)
            {
                CreateRotationYMatrix(mRotY, a);
                Traspose(mRotY);

                if(rotX)
                    CreateRotationXMatrix(mRotX, a);

                if(rotZ)
                {    
                    CreateRotationZMatrix(mRotZ, a);
                    Traspose(mRotZ);
                }

                Mat4Product(mRotX, mRotY, world1);
                Mat4Product(world1, mRotZ, world);
            }
            else
            {
                if(rotX)
                {
                    CreateRotationXMatrix(mRotX, a);

                    if(rotY)
                    {    
                        CreateRotationYMatrix(mRotY, a);
                        Traspose(mRotY);
                    }
                            
                    if(rotZ)
                    {    
                        CreateRotationZMatrix(mRotZ, a);
                        Traspose(mRotZ);
                    }        

                    Mat4Product(mRotX, mRotY, world1);
                    Mat4Product(world1, mRotZ, world);
                }
                else
                {
                    if(rotZ)
                    {
                        CreateRotationZMatrix(world, a);
                        Traspose(world);
                    }
                }
            }

            Mat4Product(world, t, world1);

            Mat4Product(world1, scale, world);

            Mat4Product(world,view, wv);

            Mat4ProductTras(wv, proj, wvp);

            Traspose2(world, worldt);
  
            a += rotSpeed;
      		
            if(Normals.size != 0)
            {
                if(m_tex_norm)
            	   RenderFilledModel_m3(&data, screenSurface, depthBuffer, tex, lightPos); 
            }
            if (m_tex)
				RenderFilledModel_m2(&data, screenSurface, depthBuffer, tex); 

           	if (m_esq)
            	RenderFilledModel_m1(&data, screenSurface); 
	

            deltaclock = SDL_GetTicks() - startclock;
            startclock = SDL_GetTicks();
                    
            if ( deltaclock != 0 )
                currentFPS = 1000 / deltaclock;

            char buf[9];
            strcpy(buf, "FPS: ");
            snprintf(&buf[5], 4, "%d", currentFPS);

            //prom += currentFPS;

            SDL_Color clrFg = {255,0,0,0};
            if(showFPS)
                RenderText(clrFg, font, buf, screenSurface);

            if(showInfo)
                ShowInfo(screenSurface);
            
        	SDL_UpdateWindowSurface( window );

            //iteraciones++;
    	}

        //float fpsPROM = prom / 1000;
        //printf("Tiempo transcurrido: %d, FPS Promedio: %f\n", SDL_GetTicks() - startApp, fpsPROM);
  	}

    freeVec4DynamicArray(&Vertices);
    freeVec2DynamicArray(&Uvs);
    freeVec4DynamicArray(&Normals);
    freeUint3DynamicArray(&Faces);


    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyWindow( window );

    SDL_Quit();

    return 0;
}