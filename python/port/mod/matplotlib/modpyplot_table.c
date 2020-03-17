#include "modpyplot.h"

STATIC MP_DEFINE_CONST_FUN_OBJ_0(modpyplot___init___obj, modpyplot___init__);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(modpyplot_axis_obj, modpyplot_axis);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(modpyplot_bar_obj, modpyplot_bar);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(modpyplot_grid_obj, modpyplot_grid);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(modpyplot_plot_obj, modpyplot_plot);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(modpyplot_scatter_obj, modpyplot_scatter);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(modpyplot_show_obj, modpyplot_show);
STATIC MP_DEFINE_CONST_FUN_OBJ_3(modpyplot_text_obj, modpyplot_text);

STATIC const mp_rom_map_elem_t modpyplot_module_globals_table[] = {
  { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_pyplot) },
  { MP_ROM_QSTR(MP_QSTR___init__), MP_ROM_PTR(&modpyplot___init___obj) },
  { MP_ROM_QSTR(MP_QSTR_axis), MP_ROM_PTR(&modpyplot_axis_obj) },
  { MP_ROM_QSTR(MP_QSTR_bar), MP_ROM_PTR(&modpyplot_bar_obj) },
  { MP_ROM_QSTR(MP_QSTR_grid), MP_ROM_PTR(&modpyplot_grid_obj) },
  { MP_ROM_QSTR(MP_QSTR_plot), MP_ROM_PTR(&modpyplot_plot_obj) },
  { MP_ROM_QSTR(MP_QSTR_scatter), MP_ROM_PTR(&modpyplot_scatter_obj) },
  { MP_ROM_QSTR(MP_QSTR_show), MP_ROM_PTR(&modpyplot_show_obj) },
  { MP_ROM_QSTR(MP_QSTR_text), MP_ROM_PTR(&modpyplot_text_obj) },
};

STATIC MP_DEFINE_CONST_DICT(modpyplot_module_globals, modpyplot_module_globals_table);

const mp_obj_module_t modpyplot_module = {
  .base = { &mp_type_module },
  .globals = (mp_obj_dict_t*)&modpyplot_module_globals,
};
