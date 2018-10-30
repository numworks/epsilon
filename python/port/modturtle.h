#include "py/obj.h"

mp_obj_t turtle_forward(mp_obj_t px);
mp_obj_t turtle_backward(mp_obj_t px);
mp_obj_t turtle_right(mp_obj_t deg);
mp_obj_t turtle_left(mp_obj_t deg);
mp_obj_t turtle_goto(size_t n_args, const mp_obj_t *args);
mp_obj_t turtle_setheading(mp_obj_t deg);
mp_obj_t turtle_speed(mp_obj_t speed);

mp_obj_t turtle_position();
mp_obj_t turtle_heading();

mp_obj_t turtle_pendown();
mp_obj_t turtle_penup();
mp_obj_t turtle_isdown();
mp_obj_t turtle_pensize(size_t n_args, const mp_obj_t *args);
mp_obj_t turtle_isvisible();

mp_obj_t turtle_color(mp_obj_t r, mp_obj_t g, mp_obj_t b);

mp_obj_t turtle_showturtle();
mp_obj_t turtle_hideturtle();

mp_obj_t turtle___init__();

void turtle_initpen(int size);
void turtle_moveto(float x, float y);
void turtle_deinit();
