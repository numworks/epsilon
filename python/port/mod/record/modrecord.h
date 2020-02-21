#include <py/obj.h>

mp_obj_t modrecord_istaken(mp_obj_t name);
mp_obj_t modrecord_save(mp_obj_t name, mp_obj_t data);
mp_obj_t modrecord_getdata(mp_obj_t name);
mp_obj_t modrecord_destroy(mp_obj_t name);
mp_obj_t modrecord_listrecords();