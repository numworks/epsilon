#include "modpyplot.h"

MP_DEFINE_CONST_FUN_OBJ_1(modpyplot_plot_obj, modpyplot_plot);
MP_DEFINE_CONST_FUN_OBJ_0(modpyplot_show_obj, modpyplot_show);

STATIC const mp_rom_map_elem_t modpyplot_module_globals_table[] = {
  { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_pyplot) },
  { MP_ROM_QSTR(MP_QSTR_plot), MP_ROM_PTR(&modpyplot_plot_obj) },
  { MP_ROM_QSTR(MP_QSTR_show), MP_ROM_PTR(&modpyplot_show_obj) },
};

STATIC MP_DEFINE_CONST_DICT(modpyplot_module_globals, modpyplot_module_globals_table);

const mp_obj_module_t modpyplot_module = {
  .base = { &mp_type_module },
  .globals = (mp_obj_dict_t*)&modpyplot_module_globals,
};
