#include "py/obj.h"
#include "py/mphal.h"

mp_obj_t mod_turtle_stamp(size_t n_args, const mp_obj_t *args);
mp_obj_t mod_turtle_draw_turtle(mp_obj_t p_x, mp_obj_t p_y, mp_obj_t p_side);
mp_obj_t mod_turtle_erase_turtle(mp_obj_t p_x, mp_obj_t p_y);

STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mod_turtle_stamp_obj, 4, 4, mod_turtle_stamp);
STATIC MP_DEFINE_CONST_FUN_OBJ_3(mod_turtle_draw_turtle_obj, mod_turtle_draw_turtle);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mod_turtle_erase_turtle_obj, mod_turtle_erase_turtle);

STATIC const mp_rom_map_elem_t mod_turtle_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR__turtle) },
    { MP_ROM_QSTR(MP_QSTR_stamp), (mp_obj_t)&mod_turtle_stamp_obj },
    { MP_ROM_QSTR(MP_QSTR_draw_turtle), (mp_obj_t)&mod_turtle_draw_turtle_obj },
    { MP_ROM_QSTR(MP_QSTR_erase_turtle), (mp_obj_t)&mod_turtle_erase_turtle_obj },
};

STATIC MP_DEFINE_CONST_DICT(mod_turtle_module_globals, mod_turtle_module_globals_table);

const mp_obj_module_t mod_turtle_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mod_turtle_module_globals,
};
