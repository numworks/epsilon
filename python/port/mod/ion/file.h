#ifndef MP_MOD_FILE_H
#define MP_MOD_FILE_H

#include <py/obj.h>

mp_obj_t file_open(mp_obj_t file_name);
mp_obj_t file_open_mode(mp_obj_t file_name, mp_obj_t file_mode);

#endif
