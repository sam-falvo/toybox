// #qo pkg-config: sdl2

#include <SDL.h>
#include "events.h"

void (*vec_button_down)(int, int);

void
event_loop(void) {
	int done;
	SDL_Event e;

	done = 0;
	while(!done) {
		SDL_WaitEvent(&e);
		if(e.type == SDL_QUIT) done++;
		else if(e.type == SDL_MOUSEBUTTONDOWN) vec_button_down(e.button.x, e.button.y);
	}
}

