#include <SDL2/SDL.h>
#include <stdio.h>
#include <unistd.h>


#define ERC_VIDEO_DRIVER_FAILED		0x0101
#define ERC_VIDEO_WINDOW		0x0102
#define ERC_VIDEO_RENDERER		0x0103


static SDL_Window *backdrop_window;
static SDL_Renderer *backdrop_renderer;


int v_init();
void v_expunge();


int
v_init() {
	int erc;

	erc = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
	if(erc) {
		erc = ERC_VIDEO_DRIVER_FAILED;
		goto micdrop;
	}
	backdrop_window = SDL_CreateWindow(
		"XIM",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		640, 480,
		SDL_WINDOW_SHOWN
	);
	if(!backdrop_window) {
		erc = ERC_VIDEO_WINDOW;
		goto micdrop;
	}
	backdrop_renderer = SDL_CreateRenderer(backdrop_window, -1, 0);
	if(!backdrop_renderer) {
		erc = ERC_VIDEO_RENDERER;
		goto micdrop;
	}

	return 0;

micdrop:
	v_expunge();
	return erc;
}


void
v_expunge() {
	if(backdrop_renderer) {
		SDL_DestroyRenderer(backdrop_renderer);
		backdrop_renderer = NULL;
	}
	if(backdrop_window) {
		SDL_DestroyWindow(backdrop_window);
		backdrop_window = NULL;
	}
	SDL_Quit();
}


int
main(int argc, char *argv[]) {
	int erc;

	erc = v_init();
	if(erc) {
		fprintf(stderr, "Video driver init failed: %04X\n", erc);
		exit(1);
	}
	sleep(5);
	v_expunge();
}

