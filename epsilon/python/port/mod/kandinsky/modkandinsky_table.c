#include "modkandinsky.h"

STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(modkandinsky_color_obj, 1, 3, modkandinsky_color);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(modkandinsky_get_pixel_obj, modkandinsky_get_pixel);
STATIC MP_DEFINE_CONST_FUN_OBJ_3(modkandinsky_set_pixel_obj, modkandinsky_set_pixel);
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(modkandinsky_draw_string_obj, 3, 5, modkandinsky_draw_string);
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(modkandinsky_fill_rect_obj, 5, 5, modkandinsky_fill_rect);

STATIC const mp_rom_map_elem_t modkandinsky_module_globals_table[] = {
  { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_kandinsky) },
  { MP_ROM_QSTR(MP_QSTR_color), (mp_obj_t)&modkandinsky_color_obj },
  { MP_ROM_QSTR(MP_QSTR_get_pixel), (mp_obj_t)&modkandinsky_get_pixel_obj },
  { MP_ROM_QSTR(MP_QSTR_set_pixel), (mp_obj_t)&modkandinsky_set_pixel_obj },
  { MP_ROM_QSTR(MP_QSTR_draw_string), (mp_obj_t)&modkandinsky_draw_string_obj },
  { MP_ROM_QSTR(MP_QSTR_fill_rect), (mp_obj_t)&modkandinsky_fill_rect_obj },
};

STATIC MP_DEFINE_CONST_DICT(modkandinsky_module_globals, modkandinsky_module_globals_table);

const mp_obj_module_t modkandinsky_module = {
  .base = { &mp_type_module },
  .globals = (mp_obj_dict_t*)&modkandinsky_module_globals,
};
