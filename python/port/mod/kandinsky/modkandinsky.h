#include <py/obj.h>

mp_obj_t modkandinsky_color(size_t n_args, const mp_obj_t *args);
mp_obj_t modkandinsky_get_pixel(mp_obj_t x, mp_obj_t y);
mp_obj_t modkandinsky_set_pixel(mp_obj_t x, mp_obj_t y, mp_obj_t color);
mp_obj_t modkandinsky_draw_string(size_t n_args, const mp_obj_t *args);
mp_obj_t modkandinsky_fill_rect(size_t n_args, const mp_obj_t *args);
