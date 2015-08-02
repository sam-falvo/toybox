// #qo pkg-config: sdl2

#include <SDL.h>
#include "workstation.h"
#include "objects.h"
#include "events.h"

static Object objs[] = {
	{1, 50, 25, 0, 0, OT_WINFRAME, 0},
	{2, 0, 0, 0, 12, OT_TITLE, "Calc"},
	{3, 0, 0, 102, 12, OT_LABEL, "0"},
	{4, 0, 14, 24, 12, OT_BUTTON, "9"},
	{5, 26, 14, 24, 12, OT_BUTTON, "8"},
	{6, 52, 14, 24, 12, OT_BUTTON, "7"},
	{7, 78, 14, 24, 12, OT_BUTTON, "/"},
	{8, 0, 28, 24, 12, OT_BUTTON, "6"},
	{9, 26, 28, 24, 12, OT_BUTTON, "5"},
	{10, 52, 28, 24, 12, OT_BUTTON, "4"},
	{11, 78, 28, 24, 12, OT_BUTTON, "*"},
	{12, 0, 42, 24, 12, OT_BUTTON, "3"},
	{13, 26, 42, 24, 12, OT_BUTTON, "2"},
	{14, 52, 42, 24, 12, OT_BUTTON, "1"},
	{15, 78, 42, 24, 12, OT_BUTTON, "-"},
	{16, 0, 56, 24, 12, OT_BUTTON, "."},
	{17, 26, 56, 24, 12, OT_BUTTON, "0"},
	{18, 52, 56, 24, 12, OT_BUTTON, "="},
	{19, 78, 56, 24, 12, OT_BUTTON, "+"},

	{19, 0, 0, 640, 12, OT_LABEL, "Calculator Version 1.0"},
};

void
on_button_down(int mx, int my) {
	int i;

	i = obj_find(objs, 1, mx, my);
	printf("Clicked on object %d at (%d, %d)\n", i, mx, my);
}

int
main(int argc, char *argv[]) {
	Workstation *wk;
	int erc, i;
	void (*old_vec_button_down)(int, int);

	erc = workstation_open(&wk);
	if(erc) {
		printf("Problem during initialization: 0x%X\n", erc);
		return 1;
	}

	/* Paint the backdrop */
	workstation_set_fg_pen(wk, 0, 128, 0);
	workstation_rectangle(wk, 0, 0, workstation_width(wk), workstation_height(wk));

	/* Calculate the calculator's layout and draw it all in one step. */
	
	objs[1].width = objs[0].width = objs[18].left + objs[18].width + 4;
	objs[0].height = objs[1].height + objs[18].top + objs[18].height + 3;

	for(i = 1; i < 19; i++) {
		objs[i].left = objs[i].left + objs[0].left + 2;
		objs[i].top = objs[i].top + objs[0].top + 13;
	}

	objs[1].left -= 2;
	objs[1].top -= 13;

	obj_draw(wk, objs, 0);

	/* Establish our callbacks. */
	old_vec_button_down = vec_button_down;
	vec_button_down = on_button_down;

	/* Return to the main loop. */
	event_loop();

	/* Restore our environment before returning to OS. */
	vec_button_down = old_vec_button_down;

	workstation_close(wk);
}
