#include "py/obj.h"

/*
 * kandinsky.color(12,0,233);
 * kandinsky.getPixel(x, y);
 * kandinsky.setPixel(x, y, color);
 * kandinsky.drawString(text, x, y);
 */

mp_obj_t kandinsky_color(mp_obj_t red, mp_obj_t green, mp_obj_t blue);
mp_obj_t kandinsky_get_pixel(mp_obj_t x, mp_obj_t y);
mp_obj_t kandinsky_set_pixel(mp_obj_t x, mp_obj_t y, mp_obj_t color);
mp_obj_t kandinsky_draw_string(mp_obj_t text, mp_obj_t x, mp_obj_t y);
