#include "modkandinsky.h"

STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(modkandinsky_color_obj, 1, 3, modkandinsky_color);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(modkandinsky_get_pixel_obj, modkandinsky_get_pixel);
STATIC MP_DEFINE_CONST_FUN_OBJ_3(modkandinsky_set_pixel_obj, modkandinsky_set_pixel);
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(modkandinsky_draw_string_obj, 3, 7, modkandinsky_draw_string);
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(modkandinsky_draw_line_obj, 5, 5, modkandinsky_draw_line);
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(modkandinsky_draw_circle_obj, 4, 4, modkandinsky_draw_circle);
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(modkandinsky_fill_rect_obj, 5, 5, modkandinsky_fill_rect);
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(modkandinsky_fill_circle_obj, 4, 4, modkandinsky_fill_circle);
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(modkandinsky_fill_polygon_obj, 2, 2, modkandinsky_fill_polygon);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(modkandinsky_wait_vblank_obj, modkandinsky_wait_vblank);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(modkandinsky_get_palette_obj, modkandinsky_get_palette);

STATIC const mp_rom_map_elem_t modkandinsky_module_globals_table[] = {
  { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_kandinsky) },
  { MP_ROM_QSTR(MP_QSTR_color), (mp_obj_t)&modkandinsky_color_obj },
  { MP_ROM_QSTR(MP_QSTR_get_pixel), (mp_obj_t)&modkandinsky_get_pixel_obj },
  { MP_ROM_QSTR(MP_QSTR_set_pixel), (mp_obj_t)&modkandinsky_set_pixel_obj },
  { MP_ROM_QSTR(MP_QSTR_draw_string), (mp_obj_t)&modkandinsky_draw_string_obj },
  { MP_ROM_QSTR(MP_QSTR_draw_line), (mp_obj_t)&modkandinsky_draw_line_obj },
  { MP_ROM_QSTR(MP_QSTR_draw_circle), (mp_obj_t)&modkandinsky_draw_circle_obj },
  { MP_ROM_QSTR(MP_QSTR_fill_rect), (mp_obj_t)&modkandinsky_fill_rect_obj },
  { MP_ROM_QSTR(MP_QSTR_fill_circle), (mp_obj_t)&modkandinsky_fill_circle_obj },
  { MP_ROM_QSTR(MP_QSTR_fill_polygon), (mp_obj_t)&modkandinsky_fill_polygon_obj },
  { MP_ROM_QSTR(MP_QSTR_large_font), mp_const_false },
  { MP_ROM_QSTR(MP_QSTR_small_font), mp_const_true },
  { MP_ROM_QSTR(MP_QSTR_wait_vblank), (mp_obj_t)&modkandinsky_wait_vblank_obj },
  { MP_ROM_QSTR(MP_QSTR_get_palette), (mp_obj_t)&modkandinsky_get_palette_obj },
};

STATIC MP_DEFINE_CONST_DICT(modkandinsky_module_globals, modkandinsky_module_globals_table);

const mp_obj_module_t modkandinsky_module = {
  .base = { &mp_type_module },
  .globals = (mp_obj_dict_t*)&modkandinsky_module_globals,
};
