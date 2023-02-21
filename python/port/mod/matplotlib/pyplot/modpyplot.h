#include <py/obj.h>

mp_obj_t modpyplot___init__();
void modpyplot_gc_collect();
void modpyplot_flush_used_heap();

mp_obj_t modpyplot_arrow(size_t n_args, const mp_obj_t *args,
                         mp_map_t *kw_args);
mp_obj_t modpyplot_axis(size_t n_args, const mp_obj_t *args);
mp_obj_t modpyplot_bar(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args);
mp_obj_t modpyplot_grid(size_t n_args, const mp_obj_t *args);
mp_obj_t modpyplot_hist(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args);
mp_obj_t modpyplot_plot(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args);
mp_obj_t modpyplot_scatter(size_t n_args, const mp_obj_t *args,
                           mp_map_t *kw_args);
mp_obj_t modpyplot_text(mp_obj_t x, mp_obj_t y, mp_obj_t s);
mp_obj_t modpyplot_show();
