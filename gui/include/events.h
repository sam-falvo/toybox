#ifndef EVENTS_H
#define EVENTS_H

void (*vec_button_down)(int, int);
void (*vec_button_up)(int, int);

void event_loop(void);

#endif
