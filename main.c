#if 0
set -ex
gcc -g -I/usr/local/include/SDL2 main.c -L/usr/local/lib -lSDL2
./a.out
exit $!
#endif

#include <stdbool.h>
#include <SDL2/SDL.h>

SDL_Window *window;
SDL_Renderer *renderer;
// SDL_Texture *texture;

void render() {
   SDL_Surface* surface = SDL_GetWindowSurface(window);
   SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0x60, 0x60, 0x60));

   {
     SDL_Rect rect = {300, 100, 200, 100};
     SDL_FillRect(surface, &rect, SDL_MapRGB(surface->format, 0xb0, 0x20, 0x20));
   }
   {
     SDL_Rect rect = {900, 100, 100, 200};
     SDL_FillRect(surface, &rect, SDL_MapRGB(surface->format, 0x20, 0xb0, 0x20));
   }
   {
     SDL_Rect rect = {400, 400, 400, 150};
     SDL_FillRect(surface, &rect, SDL_MapRGB(surface->format, 0x20, 0x20, 0xb0));
   }
   SDL_UpdateWindowSurface(window);
}

void event_loop() {
	SDL_Event event = {};
    for (;;) {
    	int ret = SDL_WaitEventTimeout(&event, 1000);
	    if (ret == 0 && strlen(SDL_GetError()) > 0) {
	    	fprintf(stderr, "error '%s'\n", SDL_GetError());
	    	continue;
	    }
	    if (event.type == SDL_QUIT) {
	    	break;
	    }
    	//fprintf(stderr, "got event\n");

    	render();
    }
}
int main() {
  if (SDL_Init(SDL_INIT_EVERYTHING)) {
    fprintf(stderr, "Could not initialize SDL: %s\n", SDL_GetError());
    return false;
  }


  window = SDL_CreateWindow(
    "demo",
              SDL_WINDOWPOS_UNDEFINED,
              SDL_WINDOWPOS_UNDEFINED,
              1280,
              720,
              SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE
   );
   if (window == NULL) {
    fprintf(stderr, "could not create window: %s\n", SDL_GetError());
    return 1;
   }
/*   
   renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
   if (window == NULL) {
    fprintf(stderr, "could not create renderer: %s\n", SDL_GetError());
    return 1;
   }
   SDL_RenderSetLogicalSize(renderer, 1280, 720);
*/
   
/*
   SDL_ShowWindow(window);
   SDL_RaiseWindow(window);
   SDL_Delay(2000);
 */

   render();
   event_loop();
   return 0;
}