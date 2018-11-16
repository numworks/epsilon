#include <py/obj.h>

mp_obj_t modturtle_forward(mp_obj_t px);
mp_obj_t modturtle_backward(mp_obj_t px);
mp_obj_t modturtle_right(mp_obj_t deg);
mp_obj_t modturtle_left(mp_obj_t deg);
mp_obj_t modturtle_goto(size_t n_args, const mp_obj_t *args);
mp_obj_t modturtle_setheading(mp_obj_t deg);
mp_obj_t modturtle_speed(mp_obj_t speed);

mp_obj_t modturtle_position();
mp_obj_t modturtle_heading();

mp_obj_t modturtle_pendown();
mp_obj_t modturtle_penup();
mp_obj_t modturtle_isdown();
mp_obj_t modturtle_pensize(size_t n_args, const mp_obj_t *args);
mp_obj_t modturtle_isvisible();

mp_obj_t modturtle_color(mp_obj_t r, mp_obj_t g, mp_obj_t b);

mp_obj_t modturtle_showturtle();
mp_obj_t modturtle_hideturtle();
