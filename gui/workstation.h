#ifndef WORKSTATION_H
#define WORKSTATION_H

typedef struct Workstation Workstation;

extern int workstation_open(Workstation **);
extern void workstation_close(Workstation *);
extern void workstation_rectangle(Workstation *, int, int, int, int);
extern void workstation_set_fg_pen(Workstation *, int, int, int);
extern int workstation_width(Workstation *);
extern int workstation_height(Workstation *);
extern void workstation_hline(Workstation *, int, int, int);
extern void workstation_rectframe(Workstation *, int, int, int, int);
extern void workstation_vline(Workstation *, int, int, int);
extern void workstation_text(Workstation *, int, int, char *);
extern void workstation_char(Workstation *, int, int, char);
extern void workstation_set_bg_pen(Workstation *, int, int, int);
extern void workstation_refresh(Workstation *);

#endif

