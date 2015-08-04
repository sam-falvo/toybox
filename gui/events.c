// #qo pkg-config: sdl2

#include <SDL.h>
#include "events.h"

void (*vec_button_down)(int, int);
void (*vec_button_up)(int, int);

static void
nop_button(int mx, int my) {
}

void
event_loop(void) {
	int done;
	SDL_Event e;

	done = 0;
	while(!done) {
		SDL_WaitEvent(&e);
		if(e.type == SDL_QUIT) done++;
		else if(e.type == SDL_MOUSEBUTTONDOWN) vec_button_down(e.button.x, e.button.y);
		else if(e.type == SDL_MOUSEBUTTONUP) vec_button_up(e.button.x, e.button.y);
	}
}

