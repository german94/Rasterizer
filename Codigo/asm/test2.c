#include "sdlHelper.h"
#include "mathHelper.h"
#include "modelLoader.h"
#include "structHelper.h"
#include <float.h>
#include <stdlib.h>
#include <time.h>  

 
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
float depthBuffer[800*800];       //no deja usar las consts por declararlo globalmente
Mat4 scale;
float rotSpeed;
bool showFPS;
bool showInfo;
Vec3 lightPos;
Vec3 color = {1.0, 1.0, 1.0};

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
    rotX = false;
    rotY = true;
    rotZ = false;
    sx = 1.0f;
    sy = 1.0f;
    sz = 1.0f;
    rotSpeed = 0.01f;
    showFPS = true;
    showInfo = true;
    m_esq = true;
	m_tex = false;
	m_tex_norm = false;

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

                    case SDLK_KP_PLUS:
                    {
                        if (color[0] <= 1.0)
                        {
                        	if(color[2] < 1.0)
                                color[2] = color[2] + 0.2;
                            else
                            {
                                if(color[1] < 1.0)
                                {
                                    color[1] = color[1] + 0.2;
                                    color[2] = 0.0;
                                }
                                else
                                {
                                    if(color[0] < 1.0)
                                    {
                                        color[0] = color[0] + 0.2;
                                        color[1] = 0.0; color[2] = 0.0;
                                    }
                                }
                            }
                        }	
                        break;
                    }

                    case SDLK_KP_MINUS:
                    {
                        if (color[0] >= 0.2)
                        {
                        	if(color[2] >= 0.2)
                        		color[2] = color[2] - 0.2;
                        	else
                        	{
                        		if(color[1] >= 0.2)
                        		{
                        			color[1] = color[1] - 0.2;
                        			color[2] = 1.0;
                        		}
                        		else
                        		{
                        			if(color[0] > 0.2)
                        			{
                        				color[0] = color[0] - 0.2;
                        				color[1] = 1.0; color[2] = 1.0;
                        			}
                        		}	
                        	}		 
                        }	
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

                    case SDLK_UP:
                    {
                        if (lightPos[1] > -SCREEN_HEIGHT/2)
                            lightPos[1]-=0.05;
                        break;
                    }

                    case SDLK_LEFT:
                    {
                        if (lightPos[0] < SCREEN_WIDTH)
                            lightPos[0]+=0.05;
                        break;
                    }

                    case SDLK_DOWN:
                    {
                        if (lightPos[1] < SCREEN_HEIGHT)
                            lightPos[1]+=0.05;
                        break;
                    }

                    case SDLK_RIGHT:
                    {
                        if (lightPos[0] > -SCREEN_WIDTH/2)
                            lightPos[0]-=0.05;
                        break;
                    }

                    case SDLK_x:
                    {
                        lightPos[2]-=0.05;
                        break;
                    }

                    case SDLK_z:
                    {
                        if (lightPos[2] < 21)
                            lightPos[2]+=0.05;
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

    if(init())
    {
        CreateScaleMatrix(scale, sx, sy, sz);
        Traspose(scale);

    	screenSurface = SDL_GetWindowSurface( window );

        Vec4DynamicArray Vertices;
        Vec2DynamicArray Uvs;
        Vec4DynamicArray Normals;
        UInt3DynamicArray Faces;
        Vec2_int max_distancias; //{indice x_max, indice y_max}

        LoadModel("model.obj", &Vertices,  &Uvs,  &Normals, &Faces, max_distancias);
          
        SDL_Surface* tex;
        if(Uvs.size != 0)
        {
        	if(argc == 2)
        		tex = SDL_LoadBMP(argv[1]);
        	else
        	{
        		printf("Falta textura.\n");
        		quit = true;
        	}
        }
        else
        	tex = NULL;

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
						 			 
            initDepthBufferASM(depthBuffer, vec_max, SCREEN_WIDTH*SCREEN_HEIGHT);

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
      		
            if(Normals.size != 0)
            {
                if(m_tex_norm)
            	   RenderFilledModel_m3(color, &Vertices,  &Uvs,  &Normals, &Faces, wvp, SCREEN_WIDTH, SCREEN_HEIGHT, screenSurface, depthBuffer, worldt, tex, lightPos, max_distancias); 
            }
            if (m_tex)
				RenderFilledModel_m2(color, &Vertices,  &Uvs, &Faces, wvp, SCREEN_WIDTH, SCREEN_HEIGHT, screenSurface, depthBuffer, tex, max_distancias); 

           	if (m_esq)
            	RenderFilledModel_m1(&Vertices, &Faces, wvp, SCREEN_WIDTH, SCREEN_HEIGHT, screenSurface, max_distancias); 
	


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
    	}
  	}

    TTF_CloseFont(font);
    SDL_DestroyWindow( window );

    SDL_Quit();

    return 0;
}