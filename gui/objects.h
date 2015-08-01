// #qo pkg-config: sdl2

#ifndef OBJECTS_H
#define OBJECTS_H

#include "workstation.h"

typedef struct Object Object;

struct Object {
	unsigned short	next;
	short		left, top;
	unsigned short	width, height;
	unsigned short	type;
	void *		ptr1;
};

#define OT_NONE		0
#define OT_WINFRAME	1
#define OT_TITLE	2
#define OT_LABEL	3
#define OT_BUTTON	4
#define OT_RECT		5
#define OT_RECTFRAME	6

/* ptr1 fields are interpreted per object type:
 *
 *	OT_TITLE	string
 *	OT_LABEL	string
 *	OT_BUTTON	string
 *
 * All other types ignore ptr1.
 */

void obj_draw_windowFrame(Workstation *, int, int, int, int);
void obj_draw_title(Workstation *, int, int, int, int, char *);
void obj_draw_label(Workstation *, int, int, int, int, char *);
void obj_draw_button(Workstation *, int, int, int, int, char *);
void obj_draw_rectangle(Workstation *, int, int, int, int, unsigned int);
void obj_draw_rectframe(Workstation *, int, int, int, int, unsigned int);

void obj_draw(Workstation *, Object *, unsigned short);

#endif

