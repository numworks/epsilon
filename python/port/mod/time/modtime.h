#include <py/obj.h>

mp_obj_t modtime_sleep(mp_obj_t seconds_o);
mp_obj_t modtime_monotonic();

//
// Upsilon extensions.
//

mp_obj_t modtime_localtime(size_t n_args, const mp_obj_t *args);
mp_obj_t modtime_mktime(mp_obj_t tuple);
mp_obj_t modtime_time(void);

// Upsilon private extensions.

mp_obj_t modtime_rtcmode(void);
mp_obj_t modtime_setrtcmode(mp_obj_t mode);
mp_obj_t modtime_setlocaltime(mp_obj_t tuple);
