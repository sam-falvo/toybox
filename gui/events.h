// #qo pkg-config: sdl2

#ifndef EVENTS_H
#define EVENTS_H

void (*vec_button_down)(int, int);

void event_loop(void);

#endif
