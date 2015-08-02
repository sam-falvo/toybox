// #qo pkg-config: sdl2

#include <string.h>

#include "workstation.h"
#include "objects.h"

#define AS_UINT(x)	((unsigned int)((unsigned long)(x)))

void
obj_draw_windowFrame(Workstation *wk, int l, int t, int w, int h) {
	int r = l + w, b = t + h;

	workstation_set_fg_pen(wk, 0, 0, 0);
	workstation_rectframe(wk, l, t, r, b);
	workstation_set_fg_pen(wk, 255, 255, 255);
	workstation_rectangle(wk, l+1, t+1, r-1, b-1);
}

void
obj_draw_title(Workstation *wk, int l, int t, int w, int h, char *title) {
	int r = l+w,
	    b = t+h,
	    tx = l + (w - (8*strlen(title)))/2;

	/* tx calculation assumes 8x8 font */

	workstation_set_fg_pen(wk, 192, 192, 192);
	workstation_rectangle(wk, l, t, r, b);
	workstation_set_fg_pen(wk, 0, 0, 0);
	workstation_rectframe(wk, l, t, r, b);
	workstation_set_bg_pen(wk, 192, 192, 192);
	workstation_text(wk, tx, t+2, title);
}

void
obj_draw_label(Workstation *wk, int l, int t, int w, int h, char *content) {
	int r = l+w, b = t+h;

	workstation_set_fg_pen(wk, 255,255,255);
	workstation_rectangle(wk, l, t, r, b);
	workstation_set_fg_pen(wk, 0, 0, 0);
	workstation_set_bg_pen(wk, 255,255,255);
	workstation_text(wk, l+2, t+2, content);
	workstation_rectframe(wk, l, t, r, b);
}

void
obj_draw_button(Workstation *wk, int l, int t, int w, int h, char *content) {
	int r = l+w, b = t+h;
	int tx = l + (w - (8*strlen(content)))/2;

	workstation_set_fg_pen(wk, 255,255,255);
	workstation_rectangle(wk, l, t, r, b);
	workstation_set_fg_pen(wk, 0, 0, 0);
	workstation_set_bg_pen(wk, 255,255,255);
	workstation_text(wk, tx, t+2, content);
	workstation_rectframe(wk, l, t, r, b);
}

void
obj_draw_rectangle(Workstation *wk, int l, int t, int w, int h, unsigned int fgpen) {
	int r = (fgpen >> 16) & 0xFF, g = (fgpen >> 8) & 0xFF, b = (fgpen) & 0xFF;

	workstation_set_fg_pen(wk, r, g, b);
	workstation_rectangle(wk, l, t, l+w, t+h);
}

void
obj_draw_rectframe(Workstation *wk, int l, int t, int w, int h, unsigned int fgpen) {
	int r = (fgpen >> 16) & 0xFF, g = (fgpen >> 8) & 0xFF, b = (fgpen) & 0xFF;

	workstation_set_fg_pen(wk, r, g, b);
	workstation_rectframe(wk, l, t, l+w, t+h);
}

void
obj_draw(Workstation *wk, Object *objects, unsigned short first) {
	int l, t, w, h;
	Object *o;

	do {
		o = &objects[first];
		l = o->left;
		t = o->top;
		w = o->width;
		h = o->height;

		switch(o->type) {
		case OT_WINFRAME:
			obj_draw_windowFrame(wk, l, t, w, h);
			break;
		case OT_TITLE:
			obj_draw_title(wk, l, t, w, h, (char *)(o->ptr1));
			break;
		case OT_LABEL:
			obj_draw_label(wk, l, t, w, h, (char *)(o->ptr1));
			break;
		case OT_BUTTON:
			obj_draw_button(wk, l, t, w, h, (char *)(o->ptr1));
			break;
		case OT_RECT:
			obj_draw_rectangle(wk, l, t, w, h, AS_UINT(o->ptr1));
			break;
		case OT_RECTFRAME:
			obj_draw_rectframe(wk, l, t, w, h, AS_UINT(o->ptr1));
			break;
		default:
			break;
		}
		if(o->next == first)
			break;
		first = o->next;
	} while(1);
}

unsigned short
obj_find(Object *objects, unsigned short start, int mx, int my) {
	Object *o;
	int l, t, r, b;

	do {
		o = &objects[start];

		l = o->left; t = o->top;
		r = l + o->width; b = t + o->height;

		if((l <= mx) && (mx <= r) && (t <= my) && (my <= b)) return start;

		if(o->next == start) break;
		start = o->next;
	} while(1);
	return 0xFFFF;
}

