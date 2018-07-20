#include <assert.h>

#include <SDL2/SDL.h>
#include <stdio.h>
#include <unistd.h>


#define SWAP(l,r)	\
{			\
	int tmp = (r);	\
	(r) = (l);	\
	(l) = tmp;	\
}


#define ERC_VIDEO_DRIVER_FAILED		0x0101
#define ERC_VIDEO_WINDOW		0x0102
#define ERC_VIDEO_RENDERER		0x0103
#define ERC_VIDEO_BACKDROP_BUFFER	0x0104


static SDL_Window *backdrop_window;
static SDL_Renderer *backdrop_renderer;

static uint8_t fg_r, fg_g, fg_b;
static uint8_t bg_r, bg_g, bg_b;
static uint32_t fg_pen, bg_pen;
static int backdrop_width, backdrop_height;
static uint32_t *backdrop_bits;


int v_init();
void v_expunge();
void v_setfgpen(uint8_t r, uint8_t g, uint8_t b);
void v_setbgpen(uint8_t r, uint8_t g, uint8_t b);
void v_hline(uint16_t fg_pat, uint16_t bg_pat, int l, int r, int y);
void v_vline(uint16_t fg_pat, uint16_t bg_pat, int x, int t, int b);
void v_endpaint(int t, int b);


void
v_endpaint(int t, int b) {
	SDL_Surface *s;
	SDL_Texture *tex;
	SDL_Rect rectangle;
	int i;
	uint32_t *src;
	uint8_t *dst;
	size_t ct;

	if((t < 0) || (t >= backdrop_height)) return;
	if((b <= 0) || (b > backdrop_height)) return;
	if(t >= b) return;

	assert((0 <= t) && (t < backdrop_height));
	assert((0 < b) && (b <= backdrop_height));
	assert(t < b);

	rectangle.x = 0;
	rectangle.y = t;
	rectangle.w = backdrop_width;
	rectangle.h = b - t;

 	s = SDL_CreateRGBSurface(0, backdrop_width, rectangle.h, 32, 0xFF0000, 0x00FF00, 0x0000FF, 0);
 	if(!s) {
 		fprintf(stderr, "WARNING: Cannot create SDL surface to commit with.\n");
 		return;
 	}

	src = backdrop_bits + t * backdrop_width;
	dst = (uint8_t *)s->pixels;
	ct = backdrop_width * sizeof(uint32_t);

 	for(i = 0; i < rectangle.h; i++) {
 		memcpy(dst, src, ct);

		src += backdrop_width;
		dst += s->pitch;
 	}

 	tex = SDL_CreateTextureFromSurface(backdrop_renderer, s);
 	SDL_FreeSurface(s);
 	SDL_RenderCopy(backdrop_renderer, tex, NULL, &rectangle);
 	SDL_DestroyTexture(tex);
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
	backdrop_bits = (uint32_t *)(malloc(backdrop_width * backdrop_height * sizeof(uint32_t)));
	if(!backdrop_bits) {
		erc = ERC_VIDEO_BACKDROP_BUFFER;
		goto micdrop;
	}
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
	if(backdrop_bits) {
		free(backdrop_bits);
		backdrop_bits = NULL;
	}
	SDL_Quit();
}


uint32_t *
v_getscanline(int y) {
	return &(backdrop_bits[y * backdrop_width]);
}


void
v_hline(uint16_t fg_pat, uint16_t bg_pat, int l, int r, int y) {
	uint32_t *bits;
	uint16_t pat_bit;

	if((y < 0) || (y >= backdrop_height)) return;
	if((l < 0) || (l >= backdrop_width)) return;
	if((r <= 0) || (r > backdrop_width)) return;

	bits = v_getscanline(y);
	pat_bit = (uint16_t)(1 << (l & 15));
	for(int i = l; i < r; i++) {
		if(fg_pat & pat_bit) {
			bits[i] = fg_pen;
		}
		else if(bg_pat & pat_bit) {
			bits[i] = bg_pen;
		}
		// Else, do nothing; leave background intact.

		pat_bit = (pat_bit << 1) | ((pat_bit & 0x8000) >> 15);
	}
}


void
v_vline(uint16_t fg_pat, uint16_t bg_pat, int x, int t, int b) {
	// ...
}


void
v_setfgpen(uint8_t r, uint8_t g, uint8_t b) {
	fg_r = r; fg_g = g; fg_b = b;
	fg_pen = (r << 16) | (g << 8) | b;
}


void
v_setbgpen(uint8_t r, uint8_t g, uint8_t b) {
	bg_r = r; bg_g = g; bg_b = b;
	bg_pen = (r << 16) | (g << 8) | b;
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
		v_setbgpen(255^r, 255^g, 255^b);
		v_hline(0xFF, 0x5500, 0, v_getwidth(), i);
		r = r + 1;
		g = g + 2;
		b = b + 4;
	}
	v_endpaint(0, v_getheight());

	sleep(5);
	v_expunge();
}

