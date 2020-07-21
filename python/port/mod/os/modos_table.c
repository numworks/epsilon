#include "modos.h"

MP_DEFINE_CONST_FUN_OBJ_0(modos_uname_obj, modos_uname);
MP_DEFINE_CONST_FUN_OBJ_1(modos_remove_obj, modos_remove);
MP_DEFINE_CONST_FUN_OBJ_2(modos_rename_obj, modos_rename);

STATIC const mp_rom_map_elem_t modos_module_globals_table[] = {
  { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_os) },
  { MP_ROM_QSTR(MP_QSTR_uname), &modos_uname_obj},
  { MP_ROM_QSTR(MP_QSTR_remove), &modos_remove_obj},
  { MP_ROM_QSTR(MP_QSTR_rename), &modos_rename_obj},
};

STATIC MP_DEFINE_CONST_DICT(modos_module_globals, modos_module_globals_table);

const mp_obj_module_t modos_module = {
  .base = { &mp_type_module },
  .globals = (mp_obj_dict_t*)&modos_module_globals,
};
