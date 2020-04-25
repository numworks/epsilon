#include <py/obj.h>

mp_obj_t modturtle___init__();
void modturtle_gc_collect();
void modturtle_view_did_disappear();

mp_obj_t modturtle_reset();

mp_obj_t modturtle_forward(mp_obj_t px);
mp_obj_t modturtle_backward(mp_obj_t px);
mp_obj_t modturtle_right(mp_obj_t deg);
mp_obj_t modturtle_left(mp_obj_t deg);
mp_obj_t modturtle_circle(size_t n_args, const mp_obj_t *args);
mp_obj_t modturtle_goto(size_t n_args, const mp_obj_t *args);
mp_obj_t modturtle_setheading(mp_obj_t deg);
mp_obj_t modturtle_speed(size_t n_args, const mp_obj_t *args);

mp_obj_t modturtle_position();
mp_obj_t modturtle_heading();

mp_obj_t modturtle_pendown();
mp_obj_t modturtle_penup();
mp_obj_t modturtle_isdown();
mp_obj_t modturtle_pensize(size_t n_args, const mp_obj_t *args);
mp_obj_t modturtle_isvisible();

mp_obj_t modturtle_pencolor(size_t n_args, const mp_obj_t *args);
mp_obj_t modturtle_colormode(size_t n_args, const mp_obj_t *args);

mp_obj_t modturtle_showturtle();
mp_obj_t modturtle_hideturtle();
