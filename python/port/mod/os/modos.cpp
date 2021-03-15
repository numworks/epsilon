extern "C" {
#include "modos.h"
#include <string.h>
#include <py/obj.h>
#include <py/runtime.h>
#include <py/objstr.h>
#include <py/objtuple.h>
}

#include <ion/storage.h>

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

mp_obj_t modos_remove(mp_obj_t o_file_name) {

  size_t len;
  const char* file_name;
  file_name = mp_obj_str_get_data(o_file_name, &len);

  Ion::Storage::Record record = Ion::Storage::sharedStorage()->recordNamed(file_name);

  if (record == Ion::Storage::Record()) {
    mp_raise_OSError(2);
  }

  record.destroy();
  
  return mp_const_none;
}

mp_obj_t modos_rename(mp_obj_t o_old_name, mp_obj_t o_new_name) {

  size_t len;
  const char* old_name;
  const char* new_name;
  old_name = mp_obj_str_get_data(o_old_name, &len);
  new_name = mp_obj_str_get_data(o_new_name, &len);

  Ion::Storage::Record record = Ion::Storage::sharedStorage()->recordNamed(old_name);

  if (record == Ion::Storage::Record()) {
    mp_raise_OSError(2);
  }
  
  Ion::Storage::Record::ErrorStatus status = record.setName(new_name);

  switch (status) {
    case Ion::Storage::Record::ErrorStatus::NameTaken:
      mp_raise_OSError(17);
      break;
    case Ion::Storage::Record::ErrorStatus::NotEnoughSpaceAvailable:
      mp_raise_OSError(28);
      break;
    case Ion::Storage::Record::ErrorStatus::NonCompliantName:
      mp_raise_OSError(22);
      break;
    case Ion::Storage::Record::ErrorStatus::RecordDoesNotExist:
      mp_raise_OSError(2);
      break;
    default:
      break;
  }
  
  return mp_const_none;
}

mp_obj_t modos_listdir(void) {
  mp_obj_t list = mp_obj_new_list(0, NULL);

  for(size_t i = 0; i < (size_t)Ion::Storage::sharedStorage()->numberOfRecords(); i++) {
    Ion::Storage::Record record = Ion::Storage::sharedStorage()->recordAtIndex(i);
    size_t file_name_length = strlen(record.fullName());
    
    mp_obj_t file_name = mp_obj_new_str(record.fullName(), file_name_length);
    mp_obj_list_append(list, file_name);
  }
  
  return list;
}

