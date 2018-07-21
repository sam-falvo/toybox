#include <assert.h>

#include <SDL2/SDL.h>
#include <stdio.h>
#include <unistd.h>


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


int v_getheight();
int v_getwidth();
int v_init();
void v_endpaint(int t, int b);
void v_expunge();
void v_frame(uint16_t fg_pat, uint16_t bg_pat, int l, int t, int r, int b);
void v_hline(uint16_t fg_pat, uint16_t bg_pat, int l, int r, int y);
void v_setbgpen(uint8_t r, uint8_t g, uint8_t b);
void v_setfgpen(uint8_t r, uint8_t g, uint8_t b);
void v_vline(uint16_t fg_pat, uint16_t bg_pat, int x, int t, int b);


/****** Initialization and Expunge ******/


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


int
v_getheight() {
	return backdrop_height;
}


int
v_getwidth() {
	return backdrop_width;
}


/****** Drawing Primitives ******/


static void
v_clipx(int *x) {
	if(*x < 0) *x = 0;
	else if(*x > backdrop_width) *x = backdrop_width;
}


static void
v_clipy(int *y) {
	if(*y < 0) *y = 0;
	else if(*y > backdrop_height) *y = backdrop_height;
}


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
 		fprintf(stderr, "WARNING: Out of memory?!  Cannot create SDL surface to commit with.\n");
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


void
v_hline(uint16_t fg_pat, uint16_t bg_pat, int l, int r, int y) {
	uint32_t *bits;
	uint16_t pat_bit;

	v_clipx(&l);
	v_clipx(&r);
	v_clipy(&y);

	if(y >= backdrop_height) return;
	if(l >= r) return;

	assert((0 <= l) && (l < backdrop_width));
	assert((0 < r) && (r <= backdrop_width));
	assert((0 <= y) && (y < backdrop_height));
	assert(l < r);

	bits = backdrop_bits + y * backdrop_width;
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
	int i;
	uint32_t *bits;
	uint16_t pat_bit;

	v_clipx(&x);
	v_clipy(&t);
	v_clipy(&b);

	if(x >= backdrop_width) return;
	if(t >= b) return;

	assert((0 <= x) && (x < backdrop_width));
	assert((0 <= t) && (t < backdrop_height));
	assert((0 < b) && (b <= backdrop_height));
	assert(t < b);

	bits = backdrop_bits + x + (t * backdrop_width);
	pat_bit = (uint16_t)(1 << (t & 15));
	for(i = t; i < b; i++) {
		if(fg_pat & pat_bit) {
			*bits = fg_pen;
		}
		else if(bg_pat & pat_bit) {
			*bits = bg_pen;
		}
		// Else, do nothing; leave background intact.

		pat_bit = (pat_bit << 1) | ((pat_bit & 0x8000) >> 15);
		bits += backdrop_width;
	}
}


void
v_frame(uint16_t fg_pat, uint16_t bg_pat, int l, int t, int r, int b) {
	if(l >= r) return;
	if(t >= b) return;

	v_hline(fg_pat, bg_pat, l, r, t);
	v_hline(fg_pat, bg_pat, l, r, b - 1);
	v_vline(fg_pat, bg_pat, l, t, b);
	v_vline(fg_pat, bg_pat, r - 1, t, b);
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
	uint16_t fg_patterns[8] = { 0x1111, 0x4444, 0x5555, 0xFFFF, 0xFFFF, 0xAAAA, 0x2222, 0x8888 };
	int w;

	erc = v_init();
	if(erc) {
		fprintf(stderr, "Video driver init failed: %04X\n", erc);
		exit(1);
	}

	w = v_getwidth() / 2;
	r = g = b = 0;
	for(int i = 0; i < w; i++) {
		v_setfgpen(r, g, b);
		v_setbgpen(255^r, 255^g, 255^b);
		v_frame(
			fg_patterns[i & 7], ~fg_patterns[i & 7],
			i, i, v_getwidth() - i, v_getheight() - i
		);
		r = r + 1;
		g = g + 2;
		b = b + 4;
	}
	v_endpaint(0, v_getheight());

	sleep(5);

	r = g = b = 0;
	for(int i = 0; i < v_getwidth(); i++) {
		v_setfgpen(r, g, b);
		v_setbgpen(255^r, 255^g, 255^b);
		v_vline(
			fg_patterns[i & 7], ~fg_patterns[i & 7],
			i, 0, v_getheight()
		);
		r = r + 1;
		g = g + 2;
		b = b + 4;
	}
	v_endpaint(0, v_getheight());

	sleep(5);

	r = g = b = 0;
	for(int i = 0; i < v_getheight(); i++) {
		v_setfgpen(r, g, b);
		v_setbgpen(255^r, 255^g, 255^b);
		v_hline(
			fg_patterns[i & 7], ~fg_patterns[i & 7],
			0, v_getwidth(), i
		);
		r = r + 1;
		g = g + 2;
		b = b + 4;
	}
	v_endpaint(0, v_getheight());

	sleep(5);

	v_expunge();
}

