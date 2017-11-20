#include "py/obj.h"
#include "py/mphal.h"
#include "modkandinsky.h"

STATIC MP_DEFINE_CONST_FUN_OBJ_3(kandinsky_color_obj, kandinsky_color);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(kandinsky_get_pixel_obj, kandinsky_get_pixel);
STATIC MP_DEFINE_CONST_FUN_OBJ_3(kandinsky_set_pixel_obj, kandinsky_set_pixel);
STATIC MP_DEFINE_CONST_FUN_OBJ_3(kandinsky_draw_string_obj, kandinsky_draw_string);

STATIC const mp_rom_map_elem_t kandinsky_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_kandinsky) },
    { MP_ROM_QSTR(MP_QSTR_color), (mp_obj_t)&kandinsky_color_obj },
    { MP_ROM_QSTR(MP_QSTR_get_pixel), (mp_obj_t)&kandinsky_get_pixel_obj },
    { MP_ROM_QSTR(MP_QSTR_set_pixel), (mp_obj_t)&kandinsky_set_pixel_obj },
    { MP_ROM_QSTR(MP_QSTR_draw_string), (mp_obj_t)&kandinsky_draw_string_obj },
};

STATIC MP_DEFINE_CONST_DICT(kandinsky_module_globals, kandinsky_module_globals_table);

const mp_obj_module_t kandinsky_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&kandinsky_module_globals,
};
