#ifdef _MSC_VER
#include <SDL.h>
#else
#include <SDL/SDL.h>
#endif /* _MSC_VER */

#include <stdio.h>
#include <math.h>
#include <complex.h>
#ifdef NOSPIRE
#include "libnodls.h"
#else
#include <libndls.h>
#endif

#define WIDTH 320
#define HEIGHT 240

#define START_POS   -0.5
#define START_ZOOM  (WIDTH * 0.25296875f)

#define BAIL_OUT        2.0
#define FLIPS           24

#define ZOOM_FACTOR     4

SDL_Surface *surface;

void sdl_draw_mandelbrot(SDL_Surface *surface, complex double center, double zoom)
{
    int f,x,y,n;
    int maxiter = (WIDTH/2) * 0.049715909 * log10(zoom);
    complex double z, c;
    float C;

    static SDL_Rect rects[HEIGHT/FLIPS];

    fprintf(stderr, "zoom: %f\n", zoom);
    fprintf(stderr, "center point: %f %+fi\n", creal(center), 
                                              cimag(center) );
    fprintf(stderr, "iterations: %d\n", maxiter);
    SDL_ShowCursor(SDL_DISABLE);
    for (f = 0; f < FLIPS; f++)
    {
        for  (y = f; y < HEIGHT; y += FLIPS)
        {
            for (x = 0; x < WIDTH; x++)
            {
                /* Get the complex point on gauss space to be calculate */
                z = c = creal(center) + ((x - (WIDTH/2))/zoom) + 
                    ((cimag(center) + ((y - (HEIGHT/2))/zoom))*_Complex_I);

                #define X creal(z)
                #define Y cimag(z)

                /* Check if point lies within the main cardiod or 
                   in the period-2 buld */
                if ( (pow(X-.25,2) + pow(Y,2))*(pow(X,2) + (X/2) + pow(Y,2) - .1875) < pow(Y,2)/4 ||
                     pow(X+1,2) + pow(Y,2) < .0625 )
                    n = maxiter;
                else
                    /* Applies the actual mandelbrot formula on that point */
                    for (n = 0; n <= maxiter && cabs(z) < BAIL_OUT; n ++)
                        z = cpow(z, 2) + c;

                C = n - log2f(logf(cabs(z)) / M_LN2 );

                /* Paint the pixel calculated depending on the number 
                   of iterations found */
                ((Uint16*)surface->pixels)[(y*surface->w) + x] = (n >= maxiter)? 0 :
                    SDL_MapRGB( surface->format,
                    (1+sin(C*0.27 + 5))*127., (1+cos(C*0.85))*127., (1+sin(C*0.15))*127. );
            }
            rects[y/FLIPS].x = 0;
            rects[y/FLIPS].y = y;
            rects[y/FLIPS].w = WIDTH;
            rects[y/FLIPS].h = 1;
        }
        SDL_Flip(surface);
        //SDL_UpdateRects(surface, HEIGHT/FLIPS, rects);
    }
    SDL_ShowCursor(SDL_ENABLE);
}

void init(void) {
  if(SDL_Init(SDL_INIT_VIDEO) == -1) {
    printf("Couldn't initialize SDL: %s\n", SDL_GetError());
    exit(EXIT_FAILURE);
  }
  surface = SDL_SetVideoMode(320, 240, has_colors ? 16 : 8, SDL_SWSURFACE);
  if(surface == NULL) {
    printf("Couldn't initialize display: %s\n", SDL_GetError());
    SDL_Quit();
    exit(EXIT_FAILURE);
  }
  SDL_ShowCursor(SDL_ENABLE);
}

void quit(void) {
  //SDL_FreeSurface(player.sprite);
  //SDL_FreeSurface(map.tileset);
  SDL_Quit();
}

int main(int argc, char **argv)
{
    /* SDL SEtup */
  init();
  atexit(quit);

    /* Initialize variables */
    double complex center = START_POS;
    double zoom = START_ZOOM;

    sdl_draw_mandelbrot(surface, center, zoom);

    SDL_Event event;
    while(1)
    {
        SDL_PollEvent(&event);
        switch (event.type)
        {
            case SDL_QUIT:
                exit(0);
                break;
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == ' ')
                {
                    center = START_POS;
                    zoom = START_ZOOM;
                    sdl_draw_mandelbrot(surface, center, zoom);
                }
                else if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                  quit();
                  return 0;
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                center = creal(center) + ((event.button.x - (WIDTH/2))/zoom) +
                         ((cimag(center) + ((event.button.y - (HEIGHT/2))/zoom))
                          *_Complex_I);

                if (event.button.button == 1)
                    zoom *= ZOOM_FACTOR;
                else if (event.button.button == 3)
                    zoom /= ZOOM_FACTOR;

                sdl_draw_mandelbrot(surface, center, zoom);
                break;
        }
    }

    quit();
    return 0;
}
