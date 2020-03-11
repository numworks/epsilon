#include <py/obj.h>

mp_obj_t modpyplot___init__();
void modpyplot_gc_collect();

mp_obj_t modpyplot_axis(mp_obj_t arg);
mp_obj_t modpyplot_plot(mp_obj_t x, mp_obj_t y);

// axis(*args, emit=True, **kwargs)
//mp_obj_t grid();
//mp_obj_t scatter(mp_obj_t x, mp_obj_t y);
//mp_obj_t bar();
//mp_obj_t hist();
//mp_obj_t axis(mp_obj_t v);
//mp_obj_t text(mp_obj_t x, mp_obj_t y, mp_obj_t text);
mp_obj_t modpyplot_show();
