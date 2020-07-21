extern "C" {
#include "modos.h"
#include <py/objstr.h>
#include <py/objtuple.h>
}

#ifndef OMEGA_VERSION
#error This file expects OMEGA_VERSION to be defined
#endif

#ifndef EPSILON_VERSION
#error This file expects EPSILON_VERSION to be defined
#endif


STATIC const MP_DEFINE_STR_OBJ(modos_uname_info_sysname_obj, "NumWorks");
STATIC const MP_DEFINE_STR_OBJ(modos_uname_info_nodename_obj, "");

STATIC const MP_DEFINE_STR_OBJ(modos_uname_info_release_obj, "O" MP_STRINGIFY(OMEGA_VERSION) "E-" MP_STRINGIFY(EPSILON_VERSION));
STATIC const MP_DEFINE_STR_OBJ(modos_uname_info_version_obj, MICROPY_VERSION_STRING);

#if defined(DEVICE_N0110)
STATIC const MP_DEFINE_STR_OBJ(modos_uname_info_machine_obj, "NumWorks N0110");
#elif defined(DEVICE_N0100)
STATIC const MP_DEFINE_STR_OBJ(modos_uname_info_machine_obj, "NumWorks N0100");
#else
STATIC const MP_DEFINE_STR_OBJ(modos_uname_info_machine_obj, "NumWorks Simulator");
#endif

STATIC const mp_rom_map_elem_t modos_uname_info_table[] = {
  { MP_ROM_QSTR(MP_QSTR_sysname), &modos_uname_info_sysname_obj },
  { MP_ROM_QSTR(MP_QSTR_nodename), &modos_uname_info_nodename_obj },
  { MP_ROM_QSTR(MP_QSTR_release), &modos_uname_info_release_obj },
  { MP_ROM_QSTR(MP_QSTR_version), &modos_uname_info_version_obj },
  { MP_ROM_QSTR(MP_QSTR_machine), &modos_uname_info_machine_obj },
};

STATIC MP_DEFINE_CONST_DICT(modos_uname_info_obj, modos_uname_info_table);

mp_obj_t modos_uname(void) {
    return (mp_obj_t)&modos_uname_info_obj;
}

