#ifndef DEF_AUXILIARES_H_
#define DEF_AUXILIARES_H_

#include "mathHelper.h"
#include "modelLoader.h"
#include "structHelper.h"

extern bool quit;
extern bool rotX;
extern bool rotY;
extern bool rotZ;
extern bool interpolate1ON;
extern float sx;
extern float sy;
extern float sz;
extern float rotSpeed;
extern bool showFPS;
extern bool showInfo;
extern bool m_esq;
extern bool m_tex;
extern bool m_tex_norm;
extern Vec3 color;
extern Mat4 scale;
extern Mat4 t;
extern Vec3 lightPos;
extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;
extern SDL_Event e;
extern float depthBuffer[800 * 800]; 
extern TTF_Font *font;
extern SDL_Window* window;
extern SDL_Surface* screenSurface;

void initDepthBuffer()
{
    int i;
    for (i = 0; i < SCREEN_WIDTH*SCREEN_HEIGHT; ++i)
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

    quit = false;
    rotX = false;
    rotY = false;
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

                    case SDLK_7:
                    {
                        interpolate1ON = !interpolate1ON;
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

                     case SDLK_KP_2:
                    {
                        t[1][3] = t[1][3] - 0.1; 
                        break;
                    }

                    case SDLK_KP_4:
                    {
                        t[0][3] = t[0][3] + 0.1;
                        break;
                    }

                    case SDLK_KP_6:
                    {
                        t[0][3] = t[0][3] - 0.1;
                        break;
                    }

                    case SDLK_KP_8:
                    {
                        t[1][3] = t[1][3] + 0.1;
                        break;
                    }

                      case SDLK_KP_7:
                    {
                        t[2][3] = t[2][3] - 0.1;
                        break;
                    }

                      case SDLK_KP_9:
                    {
                        t[2][3] = t[2][3] + 0.1;
                        break;
                    }

                    case SDLK_DOWN:
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

                    case SDLK_UP:
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
    SDL_Rect r13 = {0, 200, 0, 0};
    RenderTextR(c, font, "2, 4, 6, 7, 8, 9 (teclado numerico): mover el modelo en el espacio", screenSurface, &r13);
    SDL_Rect r14 = {0, 215, 0, 0};
    if(interpolate1ON)
        RenderTextR(c, font, "7: Interpolacion modo 1 activado", screenSurface, &r14);
    else
        RenderTextR(c, font, "7: Interpolacion modo 2 activado", screenSurface, &r14);

    SDL_Rect r15 = {0, 230, 0, 0};
    RenderTextR(c, font, "z / x: aumentar o disminuir la intensidad de la luz", screenSurface, &r15);
    SDL_Rect r16 = {0, 245, 0, 0};
    RenderTextR(c, font, "flechas: mover direccion de la luz", screenSurface, &r16);
    SDL_Rect r17 = {0, 260, 0, 0};
    RenderTextR(c, font, "+ / - : cambiar de color ", screenSurface, &r17);
}

#endif