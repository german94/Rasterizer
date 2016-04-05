#include "sdlHelper.h"
#include "mathHelper.h"
#include "modelLoader.h"
#include "structHelper.h"
#include <float.h>
#include <stdlib.h>
#include <time.h>  

 
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

extern void initDepthBufferASM(float *buffer, int tam);
extern void initDepthBufferASM2(float *buffer, float *maxs, int tam);



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
float depthBuffer[800 * 600];       //no deja usar las consts por declararlo globalmente
Mat4 scale;
float rotSpeed;
bool showFPS;
bool showInfo;

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

    quit = false;
    rotX = true;
    rotY = false;
    rotZ = false;
    sx = 1.0f;
    sy = 1.0f;
    sz = 1.0f;
    rotSpeed = 0.01f;
    showFPS = true;
    showInfo = true;
    m_esq = false;
	m_tex = false;
	m_tex_norm = true;

    return true;
}

void EventDetection()
{
    while(SDL_PollEvent(&e) != 0)
    {
        switch(e.type)
        {
            case SDL_QUIT:
            {
               quit = true;
               break;
            }

            case SDL_KEYUP:
            {
                SDL_Keycode kp = e.key.keysym.sym;
                switch(kp)
                {
                    case SDLK_ESCAPE:
                    {
                        quit = true;
                        break;
                    }

                    case SDLK_1:
                    {
                        rotX = !rotX;
                        break;
                    }

                    case SDLK_2:
                    {
                        rotY = !rotY;
                        break;
                    }

                    case SDLK_3:
                    {
                        rotZ = !rotZ;
                        break;
                    }

                    case SDLK_4:
                    {
                        m_esq = !m_esq;
                        m_tex = false;
                        m_tex_norm = false;
                        break;
                    }

                    case SDLK_5:
                    {
                        m_tex = !m_tex;
                        m_esq = false;
                        m_tex_norm = false;
                        break;
                    }

                    case SDLK_6:
                    {
                        m_tex_norm = !m_tex_norm;
                        m_esq = false;
                        m_tex = false;
                        break;
                    }

                    case SDLK_i:
                    {
                        showInfo = !showInfo;
                        break;
                    }

                    case SDLK_f:
                    {
                        showFPS = !showFPS;
                        break;
                    }

                    default:
                        break;
                }
            }

            case SDL_KEYDOWN:
            {
                SDL_Keycode kp = e.key.keysym.sym;
                switch(kp)
                {
                    case SDLK_q:
                    {
                        sx += 0.01f;
                        CreateScaleMatrix(scale, sx, sy, sz);
                        Traspose(scale);
                        break;
                    }

                    case SDLK_w:
                    {
                        sy += 0.01f;
                        CreateScaleMatrix(scale, sx, sy, sz);
                        Traspose(scale);
                        break;
                    }

                    case SDLK_e:
                    {
                        sz += 0.01f;
                        CreateScaleMatrix(scale, sx, sy, sz);
                        Traspose(scale);
                        break;
                    }

                    case SDLK_a:
                    {
                        sx -= 0.01f;
                        CreateScaleMatrix(scale, sx, sy, sz);
                        Traspose(scale);
                        break;
                    }

                    case SDLK_s:
                    {
                        sy -= 0.01f;
                        CreateScaleMatrix(scale, sx, sy, sz);
                        Traspose(scale);
                        break;
                    }

                    case SDLK_d:
                    {
                        sz -= 0.01f;
                        CreateScaleMatrix(scale, sx, sy, sz);
                        Traspose(scale);
                        break;
                    }

                    case SDLK_m:
                    {
                        rotSpeed += 0.001f;
                        break;
                    }

                    case SDLK_n:
                    {
                        rotSpeed -= 0.001f;
                        break;
                    }

                    default:
                        break;
                }
            }

            default:
                break;
        }
    }
}

void ShowInfo(SDL_Surface* screen)
{
    SDL_Color c = { 0, 255, 0, 0 };
    SDL_Rect r1 = {0, 20, 0, 0};
    RenderTextR(c, font, "1: rotar el modelo sobre el eje X", screenSurface, &r1);
    SDL_Rect r2 = {0, 35, 0, 0};
    RenderTextR(c, font, "2: rotar el modelo sobre el eje Y", screenSurface, &r2);
    SDL_Rect r3 = {0, 50, 0, 0};
    RenderTextR(c, font, "3: rotar el modelo sobre el eje Z", screenSurface, &r3);
    SDL_Rect r4 = {0, 65, 0, 0};
    RenderTextR(c, font, "q / a: aumentar o disminuir el ancho del objeto", screenSurface, &r4);
    SDL_Rect r5 = {0, 80, 0, 0};
    RenderTextR(c, font, "w / s: aumentar o disminuir el alto del objeto", screenSurface, &r5);
    SDL_Rect r6 = {0, 95, 0, 0};
    RenderTextR(c, font, "e / d: aumentar o disminuir la profunidad del objeto", screenSurface, &r6);
    SDL_Rect r7 = {0, 110, 0, 0};
    RenderTextR(c, font, "m / n: aumentar o disminuir la velocidad de rotacion", screenSurface, &r7);
    SDL_Rect r8 = {0, 125, 0, 0};
    RenderTextR(c, font, "f: mostrar u ocultar fps", screenSurface, &r8);
    SDL_Rect r9 = {0, 140, 0, 0};
    RenderTextR(c, font, "i: mostrar u ocultar informacion", screenSurface, &r9);
    SDL_Rect r10 = {0, 155, 0, 0};
    RenderTextR(c, font, "4: mostrar modo esqueleto", screenSurface, &r10);
    SDL_Rect r11 = {0, 170, 0, 0};
    RenderTextR(c, font, "5: mostrar modo texturas", screenSurface, &r11);
    SDL_Rect r12 = {0, 185, 0, 0};
    RenderTextR(c, font, "6: mostrar modo texturas y normales", screenSurface, &r12);
}

int main( int argc, char* args[] )
{

    Mat4 view, proj, worldToCamera;

    float angleOfView = 0.78; 
    float near = 0.1; 
    float far = 100; 

    Vec4 vec_max = {FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX};
    Vec3 up = { 0, 1, 0 };
    Vec3 target = { 0, 0, -1 };
    Vec3 pos = { 0, 0, 3};

    CreateViewMatrix(view, target, pos, up);
    Traspose(view);
   
    CreateProjectionMatrix(proj, near, far, angleOfView, 1.0f);
    Traspose(proj);

    float a = 0;

    Uint32 startclock = 0;
    Uint32 deltaclock = 0;
    Uint32 currentFPS = 0;

    if(init())
    {
        CreateScaleMatrix(scale, sx, sy, sz);
        Traspose(scale);

    	screenSurface = SDL_GetWindowSurface( window );

        Vec4DynamicArray Vertices;
        Vec2DynamicArray Uvs;
        Vec4DynamicArray Normals;
        UInt3DynamicArray Faces;

        LoadModel("model.obj", &Vertices,  &Uvs,  &Normals, &Faces);
          
        SDL_Surface* tex;
        if(Uvs.size != 0)
        	tex = SDL_LoadBMP("a.bmp");
        else
        	tex = NULL;

        clock_t start = clock();
        Mat4 t;
	
        CreateTranslationMatrix(t, 0.0f, 0.0f, 20.0f);
        Traspose(t);      
        
        Mat4 worldt;

        int contador = 0;
		while(!quit)
		{
			contador ++;

            startclock = SDL_GetTicks();

			EventDetection();
						 			 
            initDepthBufferASM2(depthBuffer, vec_max, SCREEN_WIDTH*SCREEN_HEIGHT);

            SDL_FillRect( screenSurface, NULL, SDL_MapRGB( screenSurface->format, 0x00, 0x00, 0x00 ) );

            Mat4 mRotX, mRotY, mRotZ;
            Identity(mRotX);
            Identity(mRotY);
            Identity(mRotZ);
            Mat4 world1, world, wv, wvp;
            Identity(world);

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
      		
            if(m_tex_norm)
            	RenderFilledModel(&Vertices,  &Uvs,  &Normals, &Faces, wvp, SCREEN_WIDTH, SCREEN_HEIGHT, screenSurface, depthBuffer, worldt, tex); 
           
            if (m_tex)
				RenderFilledModel_tex(&Vertices,  &Uvs, &Faces, wvp, SCREEN_WIDTH, SCREEN_HEIGHT, screenSurface, depthBuffer, tex); 

           	if (m_esq)
            	RenderFilledModel_esq(&Vertices, &Faces, wvp, SCREEN_WIDTH, SCREEN_HEIGHT, screenSurface); 
	


            deltaclock = SDL_GetTicks() - startclock;
            startclock = SDL_GetTicks();
                    
            if ( deltaclock != 0 )
                currentFPS = 1000 / deltaclock;

            char buf[9];
            strcpy(buf, "FPS: ");
            snprintf(&buf[5], 4, "%d", currentFPS);

            SDL_Color clrFg = {255,0,0,0};
            if(showFPS)
                RenderText(clrFg, font, buf, screenSurface);

            if(showInfo)
                ShowInfo(screenSurface);
            

        	SDL_UpdateWindowSurface( window );

        	if(contador == 3000) {printf("Tiempo transcurrido: %f\n", ((double)clock() - start) / CLOCKS_PER_SEC); quit = true;}
    	}
  	}

    TTF_CloseFont(font);
    SDL_DestroyWindow( window );

    SDL_Quit();

    return 0;
}