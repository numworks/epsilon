#include "py/obj.h"

mp_obj_t turtle_forward(mp_obj_t px);
mp_obj_t turtle_backward(mp_obj_t px);
mp_obj_t turtle_right(mp_obj_t deg);
mp_obj_t turtle_left(mp_obj_t deg);
mp_obj_t turtle_goto(mp_obj_t x, mp_obj_t y);
mp_obj_t turtle_setheading(mp_obj_t deg);
mp_obj_t turtle_speed(mp_obj_t speed);

mp_obj_t turtle_pendown();
mp_obj_t turtle_penup();
mp_obj_t turtle_pensize(size_t n_args, const mp_obj_t *args);

mp_obj_t turtle_color(mp_obj_t r, mp_obj_t g, mp_obj_t b);

mp_obj_t turtle_showturtle();
mp_obj_t turtle_hideturtle();

mp_obj_t turtle___init__();

void turtle_initpen(int size);
void turtle_deinit();
