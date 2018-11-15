#include <py/obj.h>

mp_obj_t modkandinsky_color(mp_obj_t red, mp_obj_t green, mp_obj_t blue);
mp_obj_t modkandinsky_get_pixel(mp_obj_t x, mp_obj_t y);
mp_obj_t modkandinsky_set_pixel(mp_obj_t x, mp_obj_t y, mp_obj_t color);
mp_obj_t modkandinsky_draw_string(mp_obj_t text, mp_obj_t x, mp_obj_t y);
