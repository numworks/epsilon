#include "modion.h"

MP_DEFINE_CONST_FUN_OBJ_1(modion_keyboard_keydown_obj, modion_keyboard_keydown);

STATIC const mp_rom_map_elem_t modion_module_globals_table[] = {
  { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_ion) },
  { MP_ROM_QSTR(MP_QSTR_keydown), MP_ROM_PTR(&modion_keyboard_keydown_obj) },
};

STATIC MP_DEFINE_CONST_DICT(modion_module_globals, modion_module_globals_table);

const mp_obj_module_t modion_module = {
  .base = { &mp_type_module },
  .globals = (mp_obj_dict_t*)&modion_module_globals,
};
