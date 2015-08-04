// #qo pkg-config: sdl2

#include <SDL.h>
#include "workstation.h"
#include "objects.h"
#include "events.h"

unsigned char close_icon[] = {
	0xFF, 0xFF,
	0x80, 0x01,
	0x88, 0x11,
	0x84, 0x21,
	0x82, 0x41,
	0x81, 0x81,
	0x81, 0x81,
	0x82, 0x41,
	0x84, 0x21,
	0x88, 0x11,
	0x80, 0x01,
	0xFF, 0xFF,
};

static Object objs[] = {
	{1, 50, 25, 0, 0, OT_WINFRAME, 0},
	{2, 16, 0, 0, 12, OT_TITLE, "Calc"},
	{3, 0, 0, 16, 12, OT_BITMAP, (char *)&close_icon},
	{4, 0, 0, 102, 12, OT_LABEL, "0"},
	{5, 0, 14, 24, 12, OT_BUTTON, "9"},
	{6, 26, 14, 24, 12, OT_BUTTON, "8"},
	{7, 52, 14, 24, 12, OT_BUTTON, "7"},
	{8, 78, 14, 24, 12, OT_BUTTON, "/"},
	{9, 0, 28, 24, 12, OT_BUTTON, "6"},
	{10, 26, 28, 24, 12, OT_BUTTON, "5"},
	{11, 52, 28, 24, 12, OT_BUTTON, "4"},
	{12, 78, 28, 24, 12, OT_BUTTON, "*"},
	{13, 0, 42, 24, 12, OT_BUTTON, "3"},
	{14, 26, 42, 24, 12, OT_BUTTON, "2"},
	{15, 52, 42, 24, 12, OT_BUTTON, "1"},
	{16, 78, 42, 24, 12, OT_BUTTON, "-"},
	{17, 0, 56, 24, 12, OT_BUTTON, "."},
	{18, 26, 56, 24, 12, OT_BUTTON, "0"},
	{19, 52, 56, 24, 12, OT_BUTTON, "="},
	{20, 78, 56, 24, 12, OT_BUTTON, "+"},

	{20, 0, 0, 640, 12, OT_LABEL, "Calculator Version 1.0"},
};

void
on_button_down(int mx, int my) {
	int i;

	i = obj_find(objs, 1, mx, my);
	printf("Clicked on object %d at (%d, %d)\n", i, mx, my);
}

void
on_button_up(int mx, int my) {
  int i;

  i = obj_find(objs, 1, mx, my);
  if(i == 2) {
    SDL_Event e;
    e.type = SDL_QUIT;
    SDL_PushEvent(&e);
  }
}

int
main(int argc, char *argv[]) {
	Workstation *wk;
	int erc, i;
	void (*old_vec_button_down)(int, int);
	void (*old_vec_button_up)(int, int);

	erc = workstation_open(&wk);
	if(erc) {
		printf("Problem during initialization: 0x%X\n", erc);
		return 1;
	}

	/* Paint the backdrop */
	workstation_set_fg_pen(wk, 0, 128, 0);
	workstation_rectangle(wk, 0, 0, workstation_width(wk), workstation_height(wk));

	/* Calculate the calculator's layout and draw it all in one step. */
	
	objs[1].width = objs[0].width = objs[19].left + objs[19].width + 4;
	objs[0].height = objs[1].height + objs[19].top + objs[19].height + 3;

	for(i = 1; i < 20; i++) {
		objs[i].left = objs[i].left + objs[0].left + 2;
		objs[i].top = objs[i].top + objs[0].top + 13;
	}

	objs[1].left -= 2;
	objs[1].top -= 13;
  objs[1].width -= 16;
	objs[2].left -= 2;
  objs[2].top -= 13;

	obj_draw(wk, objs, 0);
	workstation_refresh(wk);

	/* Establish our callbacks. */
	old_vec_button_down = vec_button_down;
  old_vec_button_up = vec_button_up;

	vec_button_down = on_button_down;
  vec_button_up = on_button_up;

	/* Return to the main loop. */
	event_loop();

	/* Restore our environment before returning to OS. */
	vec_button_down = old_vec_button_down;
	vec_button_up = old_vec_button_up;

	workstation_close(wk);
}
