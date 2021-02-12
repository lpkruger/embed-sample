#if 0
set -ex
gcc -g -I/usr/local/include/SDL2 main.c duk_console.c -L/usr/local/lib -lSDL2 -lduktape
./a.out
exit $!
#endif

#include <stdbool.h>
#include <SDL2/SDL.h>
#include <duktape.h>

#include "duk_console.h"

SDL_Window *window;
SDL_Renderer *renderer;

duk_context *ctx;
SDL_Surface* surface;

static void push_file_as_string(duk_context *ctx, const char *filename) {
    FILE *f;
    size_t len;
    char buf[4*1024*1024];

    f = fopen(filename, "rb");
    if (f) {
        len = fread((void *) buf, 1, sizeof(buf), f);
        fclose(f);
        duk_push_lstring(ctx, (const char *) buf, (duk_size_t) len);
    } else {
        fprintf(stderr, "file %s not loaded\n", filename);
        duk_push_undefined(ctx);
    }
    if (duk_get_top(ctx) > 1) {
      fprintf(stderr, "Stack has %d\n", duk_get_top(ctx));
    }
}

static int load_file(duk_context *ctx, const char *filename) {
  push_file_as_string(ctx, filename);
  duk_push_string(ctx, filename);
  return duk_pcompile(ctx, 0);
}

static int js_load_file(duk_context *ctx) {
  const char* filename = duk_safe_to_string(ctx, -1);
  int ret = load_file(ctx, filename);
  if (ret == 0) {
    duk_replace(ctx, -2);
  } else {
    duk_dup_top(ctx);
    fprintf(stderr, "compile failed: %s\n", duk_safe_to_string(ctx, -1));
    duk_pop(ctx);
  }
  return 1;
}

duk_ret_t js_draw_rectangle(duk_context* ctx) {
  SDL_Rect rect = {duk_get_int(ctx, -7), duk_get_int(ctx, -6), 
  	               duk_get_int(ctx, -5), duk_get_int(ctx, -4)};
  SDL_FillRect(surface, &rect, SDL_MapRGB(surface->format, 
  	  duk_get_int(ctx, -3), duk_get_int(ctx, -2), duk_get_int(ctx, -1)));
  return 0;
}

void render() {
   surface = SDL_GetWindowSurface(window);
   SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0x60, 0x60, 0x60));

   int ret = load_file(ctx, "render.js");
  if (ret != 0) {
    fprintf(stderr, "compile failed: %s\n", duk_safe_to_string(ctx, -1));
  } else {
    //duk_dup_top(ctx);
    //printf("compiled program: %s\n", duk_safe_to_string(ctx, -1));
    //duk_pop(ctx);
    if (duk_pcall(ctx, 0) != 0) {      /* [ func ] -> [ result ] */
      fprintf(stderr, "program error: %s\n", duk_safe_to_string(ctx, -1));
    } else {
      if (!duk_is_undefined(ctx, -1)) {
        fprintf(stderr, "program result: %s\n", duk_safe_to_string(ctx, -1));
      }
    }
  }
  duk_pop(ctx);

  SDL_UpdateWindowSurface(window);
}

void event_loop() {
	SDL_Event event = {};
    for (;;) {
    	int ret = SDL_WaitEvent(&event);
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
  
  ctx = duk_create_heap_default();
  duk_console_init(ctx, DUK_CONSOLE_PROXY_WRAPPER /*flags*/);
  duk_push_c_function(ctx, (duk_c_function) js_draw_rectangle, 7);
  duk_put_global_string(ctx, "draw_rectangle");
  duk_push_c_function(ctx, js_load_file, 1);
  duk_put_global_string(ctx, "load_file");

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
