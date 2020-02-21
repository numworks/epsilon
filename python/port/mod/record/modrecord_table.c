#include "modrecord.h"

STATIC MP_DEFINE_CONST_FUN_OBJ_1(modrecord_istaken_obj, modrecord_istaken);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(modrecord_save_obj, modrecord_save);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(modrecord_getdata_obj, modrecord_getdata);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(modrecord_destroy_obj, modrecord_destroy);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(modrecord_listrecords_obj, modrecord_listrecords);

STATIC const mp_rom_map_elem_t modrecord_module_globals_table[] = {
    {MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_record)},
    {MP_ROM_QSTR(MP_QSTR_istaken), (mp_obj_t)&modrecord_istaken_obj},
    {MP_ROM_QSTR(MP_QSTR_save), (mp_obj_t)&modrecord_save_obj},
    {MP_ROM_QSTR(MP_QSTR_getdata), (mp_obj_t)&modrecord_getdata_obj},
    {MP_ROM_QSTR(MP_QSTR_destroy), (mp_obj_t)&modrecord_destroy_obj},
    {MP_ROM_QSTR(MP_QSTR_listrecords), (mp_obj_t)&modrecord_listrecords_obj},
};

STATIC MP_DEFINE_CONST_DICT(modrecord_module_globals, modrecord_module_globals_table);

const mp_obj_module_t modrecord_module = {
  .base = { &mp_type_module },
  .globals = (mp_obj_dict_t*)&modrecord_module_globals,
};
