// #qo pkg-config: sdl2

#include <SDL.h>
#include "workstation.h"

#define WORKSTATION_ERROR(n)	((n)|0x1000)

struct Workstation {
	SDL_Window *wBackdrop;
	SDL_Renderer *rBackdrop;

	int width, height;
	int fgr, fgg, fgb;	/* foreground drawing pen */
	int bgr, bgg, bgb;	/* background drawing pen */
};


/* Stored as a 2048x8 bitmap. */
static unsigned short system_font[] = {
	0x00f0, 0x0f00, 0x000f, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0018, 0x666c, 0x1870, 0x3818, 0x0c30, 0x0000, 0x0000, 0x0000,
	0x1808, 0x3c3c, 0x0c7e, 0x1c7e, 0x3c3c, 0x0000, 0x0400, 0x203c,
	0x3c10, 0x7c1e, 0x787e, 0x7e3c, 0x663c, 0x0666, 0x6042, 0x663c,
	0x7c3c, 0x7c3c, 0x7e66, 0xc666, 0x6666, 0x7e3c, 0x003c, 0x1800,
	0x3000, 0x6000, 0x0600, 0x1c00, 0x6000, 0x0060, 0x1800, 0x0000,
	0x0000, 0x0000, 0x0800, 0x0000, 0x0000, 0x000c, 0x1830, 0x36aa,
	0x8040, 0x2010, 0x0804, 0x0201, 0xff00, 0x0000, 0x0000, 0x0000,
	0xffff, 0xffc0, 0xe0f0, 0x8001, 0x8001, 0xffff, 0x0004, 0x0020,
	0x0010, 0x1018, 0x0044, 0x1038, 0x2838, 0x3800, 0x0000, 0x387c,
	0x1010, 0x3838, 0x0800, 0x7c00, 0x0010, 0x3800, 0x3030, 0xc010,
	0x2008, 0x1034, 0x2810, 0x2e38, 0x2008, 0x1028, 0x2008, 0x1028,
	0x7034, 0x2008, 0x1034, 0x2800, 0x3c20, 0x0810, 0x2808, 0x2030,
	0x1008, 0x1034, 0x2810, 0x0000, 0x2008, 0x1028, 0x2008, 0x1028,
	0x4834, 0x2008, 0x1034, 0x2800, 0x0220, 0x0810, 0x2808, 0x4028,
	0x00f0, 0x0f00, 0x000f, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0018, 0x666c, 0x3e54, 0x6c18, 0x1818, 0x2418, 0x0000, 0x0006,
	0x2418, 0x6666, 0x1c40, 0x3006, 0x6666, 0x1818, 0x0c00, 0x3066,
	0x6638, 0x6630, 0x6c60, 0x6060, 0x6618, 0x066c, 0x6066, 0x7666,
	0x6666, 0x6666, 0x1866, 0xc666, 0x6666, 0x0630, 0x600c, 0x1800,
	0x1800, 0x6000, 0x0600, 0x3000, 0x6018, 0x1860, 0x1800, 0x0000,
	0x0000, 0x0000, 0x1800, 0x0000, 0x0000, 0x0018, 0x1818, 0x6c55,
	0x8040, 0x2010, 0x0804, 0x0201, 0x00ff, 0x0000, 0x0000, 0x0000,
	0xffff, 0xffc0, 0xe0f0, 0x4002, 0xc003, 0x7ffe, 0x0004, 0x0020,
	0x0000, 0x3820, 0x4444, 0x1040, 0x0054, 0x1c14, 0x0000, 0x7400,
	0x2810, 0x0404, 0x1000, 0xf400, 0x0030, 0x4450, 0x1010, 0x2000,
	0x1010, 0x2858, 0x0028, 0x5844, 0x1010, 0x2800, 0x1010, 0x2800,
	0x4858, 0x1010, 0x2858, 0x0044, 0x4c10, 0x1028, 0x0010, 0x2048,
	0x0810, 0x2858, 0x0028, 0x0000, 0x1010, 0x2800, 0x1010, 0x2800,
	0x3058, 0x1010, 0x2858, 0x0010, 0x3c10, 0x1028, 0x0010, 0x4000,
	0x00f0, 0x0f00, 0x000f, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0018, 0x24fe, 0x5868, 0x6818, 0x300c, 0x1818, 0x0000, 0x000c,
	0x6638, 0x0606, 0x3c7c, 0x600c, 0x6666, 0x1818, 0x187e, 0x1806,
	0x6e38, 0x6660, 0x6660, 0x6060, 0x6618, 0x0678, 0x607e, 0x7666,
	0x6666, 0x6660, 0x1866, 0x6c66, 0x3c3c, 0x0c30, 0x300c, 0x3c00,
	0x0c3c, 0x7c3e, 0x3e3c, 0x303e, 0x7c00, 0x0066, 0x18fc, 0x5c3c,
	0x7c3e, 0x5e3e, 0x3c66, 0x66c6, 0x6666, 0x7e18, 0x1818, 0x00aa,
	0x8040, 0x2010, 0x0804, 0x0201, 0x0000, 0xff00, 0x0000, 0x0000,
	0x00ff, 0xffc0, 0xe0f0, 0x2004, 0xe007, 0x3ffc, 0xe004, 0x0720,
	0x0010, 0x5420, 0x3828, 0x1038, 0x006c, 0x6428, 0x7c00, 0x6c00,
	0x107c, 0x181c, 0x0044, 0xf400, 0x0010, 0x4428, 0x1010, 0xc410,
	0x0000, 0x0000, 0x0010, 0x5840, 0x7c7c, 0x7c7c, 0x3838, 0x3838,
	0x4444, 0x3838, 0x3838, 0x3828, 0x5400, 0x0000, 0x0044, 0x3850,
	0x3838, 0x3838, 0x3810, 0x6c38, 0x3838, 0x3838, 0x0000, 0x0000,
	0x0858, 0x3838, 0x3838, 0x3800, 0x4c44, 0x4400, 0x4444, 0x5844,
	0x00f0, 0x0f00, 0x000f, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0018, 0x006c, 0x3c10, 0x3200, 0x300c, 0x7e7e, 0x007e, 0x0018,
	0x6618, 0x0c1c, 0x6c06, 0x7c0c, 0x3c3e, 0x0000, 0x3000, 0x0c0c,
	0x6a6c, 0x7c60, 0x6678, 0x786e, 0x7e18, 0x0670, 0x607e, 0x7e66,
	0x7c66, 0x7c3c, 0x1866, 0x6c7e, 0x183c, 0x1830, 0x180c, 0x3c00,
	0x0006, 0x6660, 0x6666, 0x7c66, 0x6638, 0x186c, 0x18d6, 0x6666,
	0x6666, 0x6060, 0x1866, 0x66c6, 0x3c66, 0x0c60, 0x1806, 0x0055,
	0x8040, 0x2010, 0x0804, 0x0201, 0x0000, 0x00ff, 0x0000, 0x0000,
	0x0000, 0xffc0, 0xe0f0, 0x1008, 0xf00f, 0x1ff8, 0x1808, 0x1810,
	0x0010, 0x5070, 0x287c, 0x0044, 0x0064, 0x3c50, 0x0438, 0x7400,
	0x0010, 0x2004, 0x0044, 0x7410, 0x0010, 0x3814, 0x7c7c, 0x2820,
	0x1010, 0x1010, 0x1010, 0x8c40, 0x4040, 0x4040, 0x1010, 0x1010,
	0xe464, 0x4444, 0x4444, 0x4410, 0x5444, 0x4444, 0x4428, 0x2478,
	0x0404, 0x0404, 0x0438, 0x1244, 0x4444, 0x4444, 0x3030, 0x3030,
	0x3864, 0x4444, 0x4444, 0x447c, 0x5444, 0x4444, 0x4444, 0x6444,
	0x0000, 0x00f0, 0x0ff0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0018, 0x00fe, 0x1a2c, 0x6c00, 0x300c, 0x1818, 0x1000, 0x0030,
	0x6618, 0x3006, 0x7e06, 0x6618, 0x6606, 0x0018, 0x187e, 0x1818,
	0x667c, 0x6660, 0x6660, 0x6066, 0x6618, 0x6678, 0x6066, 0x6e66,
	0x6066, 0x7806, 0x1866, 0x387e, 0x3c18, 0x3030, 0x0c0c, 0x6600,
	0x003e, 0x6660, 0x667e, 0x3066, 0x6618, 0x1878, 0x18d6, 0x6666,
	0x6666, 0x603c, 0x1866, 0x3cd6, 0x1866, 0x1818, 0x1818, 0x00aa,
	0x8040, 0x2010, 0x0804, 0x0201, 0x0000, 0x0000, 0xff00, 0x0000,
	0x0000, 0x00c0, 0xe0f0, 0x0810, 0xf81f, 0x0ff0, 0x0818, 0x1018,
	0x0010, 0x5420, 0x3810, 0x1038, 0x006c, 0x7c28, 0x0000, 0x6c00,
	0x0010, 0x3c38, 0x0044, 0x1400, 0x0010, 0x0028, 0x0830, 0xd440,
	0x2828, 0x2828, 0x2828, 0xf840, 0x7070, 0x6070, 0x1010, 0x1010,
	0x4454, 0x4444, 0x4444, 0x4428, 0x5444, 0x4444, 0x4410, 0x3844,
	0x3c3c, 0x3c3c, 0x3c1c, 0x7e40, 0x7c7c, 0x7c7c, 0x1010, 0x1010,
	0x4444, 0x4444, 0x4444, 0x4400, 0x6444, 0x4444, 0x4444, 0x6444,
	0x0000, 0x00f0, 0x0ff0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x006c, 0x7c54, 0x6c00, 0x1818, 0x2418, 0x1800, 0x1860,
	0x2418, 0x6066, 0x0c66, 0x6618, 0x660c, 0x1818, 0x0c00, 0x3000,
	0x60c6, 0x6630, 0x6c60, 0x6066, 0x6618, 0x666c, 0x6066, 0x6e66,
	0x606e, 0x6c66, 0x1866, 0x3866, 0x6618, 0x6030, 0x060c, 0x0000,
	0x0066, 0x6660, 0x6660, 0x303e, 0x6618, 0x186c, 0x18c6, 0x6666,
	0x7c3e, 0x6006, 0x1866, 0x3cd6, 0x3c3e, 0x3018, 0x1818, 0x0055,
	0x8040, 0x2010, 0x0804, 0x0201, 0x0000, 0x0000, 0x00ff, 0x0000,
	0x0000, 0x00c0, 0xe0f0, 0x0420, 0xfc3f, 0x07e0, 0x04e0, 0x2007,
	0x0010, 0x3820, 0x447c, 0x1004, 0x0054, 0x0014, 0x0000, 0x6c00,
	0x0000, 0x0000, 0x0064, 0x1400, 0x0000, 0x7c50, 0x2808, 0x2c44,
	0x4444, 0x4444, 0x4444, 0x8844, 0x4040, 0x4040, 0x1010, 0x1010,
	0x484c, 0x4444, 0x4444, 0x4444, 0x6444, 0x4444, 0x4410, 0x2064,
	0x4444, 0x4444, 0x4464, 0x9044, 0x4040, 0x4040, 0x1010, 0x1010,
	0x4444, 0x4444, 0x4444, 0x4410, 0x7844, 0x4444, 0x443c, 0x583c,
	0x0000, 0x00f0, 0x0ff0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0018, 0x006c, 0x181c, 0x3a00, 0x0c30, 0x0000, 0x1000, 0x1800,
	0x183c, 0x7e3c, 0x0c3c, 0x3c18, 0x3c38, 0x1810, 0x0400, 0x2018,
	0x3cc6, 0x7c1e, 0x787e, 0x603c, 0x663c, 0x3c66, 0x7e66, 0x663c,
	0x603c, 0x663c, 0x183c, 0x1042, 0x6618, 0x7e3c, 0x003c, 0x007e,
	0x003a, 0x7c3e, 0x3e3c, 0x3006, 0x663c, 0x1866, 0x0cc6, 0x663c,
	0x6006, 0x607c, 0x0c3a, 0x187c, 0x6606, 0x7e0c, 0x1830, 0x00aa,
	0x8040, 0x2010, 0x0804, 0x0201, 0x0000, 0x0000, 0x0000, 0xff00,
	0x0000, 0x00c0, 0xe0f0, 0x0240, 0xfe7f, 0x03c0, 0x0400, 0x2000,
	0x0010, 0x105c, 0x0010, 0x1038, 0x0038, 0x0000, 0x0000, 0x3800,
	0x007c, 0x0000, 0x005c, 0x1400, 0x0800, 0x0000, 0x3810, 0x5c38,
	0x7c7c, 0x7c7c, 0x7c7c, 0x8e38, 0x7c7c, 0x7c7c, 0x3838, 0x3838,
	0x7044, 0x3838, 0x3838, 0x3800, 0x7838, 0x3838, 0x3810, 0x2058,
	0x3c3c, 0x3c3c, 0x3c3c, 0x6e38, 0x3838, 0x3838, 0x3838, 0x3838,
	0x3844, 0x3838, 0x3838, 0x3800, 0x8038, 0x3838, 0x3804, 0x4004,
	0x0000, 0x00f0, 0x0ff0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x2000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0020, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0006, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x003c, 0x0000, 0x7000, 0x0000, 0x0000,
	0x6006, 0x0000, 0x0000, 0x0000, 0x003c, 0x0000, 0x0000, 0x0055,
	0x8040, 0x2010, 0x0804, 0x0201, 0x0000, 0x0000, 0x0000, 0x00ff,
	0x0000, 0x00c0, 0xe0f0, 0x0180, 0xffff, 0x0180, 0x0400, 0x2000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0040, 0x0000, 0x1000, 0x0000, 0x0838, 0x0400,
	0x4444, 0x4444, 0x4444, 0x0060, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0080,
	0x0000, 0x0000, 0x0000, 0x0060, 0x0000, 0x0000, 0x0000, 0x0000,
	0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0038, 0x4038,
};

void
workstation_close(Workstation *wk) {
	if(wk) {
		/* No need to destroy surface; we didn't make it ourselves. */
		if(wk->rBackdrop) SDL_DestroyRenderer(wk->rBackdrop);
		if(wk->wBackdrop) SDL_DestroyWindow(wk->wBackdrop);
		SDL_Quit();
		free(wk);
	}
}

int
workstation_open(Workstation **pwk) {
	int erc;
	static Workstation *wk = 0;

	if(wk) {
		*pwk = wk;
		return 0;
	}

	*pwk = 0;

	wk = (Workstation *)malloc(sizeof(Workstation));
	if(!wk) return WORKSTATION_ERROR(1);
	wk->width = 640;
	wk->height = 480;

	erc = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
	if(erc) {
		erc = 2;
		goto micdrop;
	}

	wk->wBackdrop = SDL_CreateWindow(
		"GUI",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		wk->width, wk->height,
		SDL_WINDOW_SHOWN
	);
	if(!wk->wBackdrop) {
		erc = 3;
		goto micdrop;
	}

	wk->rBackdrop = SDL_CreateRenderer(wk->wBackdrop, -1, 0);
	if(!wk->rBackdrop) {
		erc = 5;
		goto micdrop;
	}

	*pwk = wk;
	return 0;

micdrop:
	workstation_close(wk);
	return erc;
}


void
workstation_set_fg_pen(Workstation *wk, int r, int g, int b) {
	wk -> fgr = r;
	wk -> fgg = g;
	wk -> fgb = b;
}

void
workstation_set_bg_pen(Workstation *wk, int r, int g, int b) {
	wk -> bgr = r;
	wk -> bgg = g;
	wk -> bgb = b;
}

int
workstation_width(Workstation *wk) {
	return wk->width;
}

int
workstation_height(Workstation *wk) {
	return wk->height;
}

void
workstation_rectangle(Workstation *wk, int l, int t, int r, int b) {
	int i;

	for(i = t; i < b; i++)
		workstation_hline(wk, l, r, i);
}

void
workstation_hline(Workstation *wk, int l, int r, int y) {
	SDL_SetRenderDrawColor(wk->rBackdrop, wk->fgr, wk->fgg, wk->fgb, 0xFF);
	SDL_RenderDrawLine(wk->rBackdrop, l, y, r-1, y);
}

void
workstation_rectframe(Workstation *wk, int l, int t, int r, int b) {
	workstation_hline(wk, l, r, t);
	workstation_hline(wk, l, r, b-1);
	workstation_vline(wk, l, t, b);
	workstation_vline(wk, r-1, t, b);
}

void
workstation_vline(Workstation *wk, int x, int t, int b) {
	SDL_SetRenderDrawColor(wk->rBackdrop, wk->fgr, wk->fgg, wk->fgb, 0xFF);
	SDL_RenderDrawLine(wk->rBackdrop, x, t, x, b-1);
}

void
workstation_text(Workstation *wk, int x, int y, char *str) {
	while(*str) {
		workstation_char(wk, x, y, *str);
		x = x + 8;
		str++;
	}
}

void
workstation_char(Workstation *wk, int x, int y, char ch) {
	SDL_Surface *s;
	SDL_Texture *t;
	unsigned int *p, fgpen, bgpen;	// longs are 64-bits!  Use ints!
	unsigned char *q, row;
	SDL_Rect dr;

	dr.x = x;
	dr.y = y;
	dr.w = 8;
	dr.h = 8;

	fgpen = (wk->fgr<<16) | (wk->fgg<<8) | (wk->fgb);
	bgpen = (wk->bgr<<16) | (wk->bgg<<8) | (wk->bgb);

	/* We XOR 1 the character code because font is stored in big-endian
	 * layout, but x86 is little-endian, and I'm too lazy to swap bytes
	 * manually. -saf2
	 */
	q = ((unsigned char *)system_font)+(ch ^ 1);

	s = SDL_CreateRGBSurface(0, 8, 8, 32, 0xFF0000, 0xFF00, 0xFF, 0);
	if(!s) return;

	p = (unsigned int *)(s->pixels);
	for(y = 0; y < 8; y++) {
		row = *q;
		q = q + 256;
		for(x = 0; x < 8; x++) {
			if(row & 0x80) {
				*p = fgpen;
			} else {
				*p = bgpen;
			}
			row <<= 1;
			p++;
		}
	}

	t = SDL_CreateTextureFromSurface(wk->rBackdrop, s);
	SDL_FreeSurface(s);
	SDL_RenderCopy(wk->rBackdrop, t, NULL, &dr);
	SDL_DestroyTexture(t);
}

void
workstation_refresh(Workstation *wk) {
  SDL_RenderPresent(wk->rBackdrop);
}

