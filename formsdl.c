#include "interpreter.h"

void initialise_SDL(SDL_win *sw)
{

  if(SDL_Init(SDL_INIT_VIDEO) != 0){ /* SDL_INIT_EVERYTHING */
    ERROR("SDL initialisation failed...\n");
  }

  sw->finished = 0;
  sw->window = SDL_CreateWindow("SDL_Window",
  SDL_WINDOWPOS_UNDEFINED, /* X Position */
  SDL_WINDOWPOS_UNDEFINED, /* Y Position */
  WIN_WIDTH, WIN_HEIGHT, /* Width and Heigt of window in screen coordinates*/
  SDL_WINDOW_SHOWN); /* Window is visible */

  if(sw->window == NULL){
    ERROR("Initialisation of SDL window failed...\n");
  }

  sw->render = SDL_CreateRenderer(sw->window, -1, SDL_RENDERER_ACCELERATED
  | SDL_RENDERER_TARGETTEXTURE);

  if(sw->render == NULL){
    ERROR("Initialisation of renderer finished...\n");
  }
  SDL_SetRenderDrawBlendMode(sw->render, SDL_BLENDMODE_BLEND);

  /* Create texture for display */
  sw->display = SDL_CreateTexture(sw->render, SDL_PIXELFORMAT_RGBA8888,
                    SDL_TEXTUREACCESS_TARGET, WIN_WIDTH, WIN_HEIGHT);

  if(sw->display == NULL){
    ERROR("Unable to initiaise SDL texture\n");
  }
  SDL_SetRenderTarget(sw->render, sw->display);

  /* Set a texture as the current rendering target */

  SDL_SetRenderDrawColor(sw->render, 0, 0, 0, 255);
  SDL_RenderClear(sw->render);
}

void SDL_UpdateScreen(SDL_win *sw)
{
  SDL_SetRenderTarget(sw->render, NULL);
  SDL_RenderCopy(sw->render, sw->display, NULL, NULL);
  SDL_RenderPresent(sw->render);
  SDL_SetRenderTarget(sw->render, sw->display);
}

int Use_SDL_Event()
{
  SDL_Event event;

  while(SDL_PollEvent(&event)){
    switch(event.type){
      case SDL_QUIT:
      case SDL_MOUSEBUTTONDOWN:
      case SDL_KEYDOWN:
        return 1;
    }
  }
  return 0;
}

void Use_SDL_SetDrawColour(SDL_win *sw, int num1, int num2, int num3)
{
  SDL_SetRenderDrawColor(sw->render, num1, num2, num3, 255); /* Colour Red */
}

void Draw_line(SDL_win *sw, int old_x, int old_y, int new_x, int new_y)
{
  SDL_RenderDrawLine(sw->render,old_x,old_y,new_x,new_y);
}
