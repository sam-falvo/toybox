#include <SDL2/SDL.h>
#include <stdio.h>
#include <unistd.h>


#define ERC_VIDEO_DRIVER_FAILED		0x0101
#define ERC_VIDEO_WINDOW		0x0102
#define ERC_VIDEO_RENDERER		0x0103


static SDL_Window *backdrop_window;
static SDL_Renderer *backdrop_renderer;

static uint8_t fg_r, fg_g, fg_b;

static int backdrop_width, backdrop_height;


int v_init();
void v_expunge();
void v_setfgpen(uint8_t r, uint8_t g, uint8_t b);
void v_hline(int l, int r, int y);
void v_vline(int x, int t, int b);
void v_endpaint();


void
v_endpaint() {
	SDL_RenderPresent(backdrop_renderer);
}


int
v_getheight() {
	return backdrop_height;
}


int
v_getwidth() {
	return backdrop_width;
}


int
v_init() {
	int erc;

	erc = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
	if(erc) {
		erc = ERC_VIDEO_DRIVER_FAILED;
		goto micdrop;
	}
	backdrop_width = 640;
	backdrop_height = 480;
	backdrop_window = SDL_CreateWindow(
		"XIM",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		backdrop_width, backdrop_height,
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


static int
clip_bounds_v(int y) {
	if(y < 0) return 0;
	if(y >= backdrop_height) return backdrop_height - 1;
	return y;
}


static int
clip_bounds_h(int x) {
	if(x < 0) return 0;
	if(x >= backdrop_width) return backdrop_width - 1;
	return x;
}


void
v_hline(int l, int r, int y) {
	y = clip_bounds_v(y);
	SDL_SetRenderDrawColor(backdrop_renderer, fg_r, fg_g, fg_b, 0xFF);
	SDL_RenderDrawLine(backdrop_renderer, clip_bounds_h(l), y, clip_bounds_h(r-1), y);
}


void
v_vline(int x, int t, int b) {
	SDL_SetRenderDrawColor(backdrop_renderer, fg_r, fg_g, fg_b, 0xFF);
	SDL_RenderDrawLine(backdrop_renderer, x, t, x, b-1);
}


void
v_setfgpen(uint8_t r, uint8_t g, uint8_t b) {
	fg_r = r; fg_g = g; fg_b = b;
}


int
main(int argc, char *argv[]) {
	int erc;
	uint8_t r, g, b;

	erc = v_init();
	if(erc) {
		fprintf(stderr, "Video driver init failed: %04X\n", erc);
		exit(1);
	}

	r = g = b = 0;
	for(int i = 0; i < v_getheight(); i++) {
		v_setfgpen(r, g, b);
		v_hline(0, v_getwidth(), i);
		r = r + 1;
		g = g + 2;
		b = b + 4;
	v_endpaint();
	}

	sleep(5);
	v_expunge();
}

