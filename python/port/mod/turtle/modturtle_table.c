#include "modturtle.h"

STATIC MP_DEFINE_CONST_FUN_OBJ_1(modturtle_forward_obj, modturtle_forward);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(modturtle_backward_obj, modturtle_backward);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(modturtle_right_obj, modturtle_right);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(modturtle_left_obj, modturtle_left);
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(modturtle_goto_obj, 1, 2, modturtle_goto);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(modturtle_setheading_obj, modturtle_setheading);
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(modturtle_circle_obj, 1, 2, modturtle_circle);
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(modturtle_speed_obj, 0, 1, modturtle_speed);

STATIC MP_DEFINE_CONST_FUN_OBJ_0(modturtle_position_obj, modturtle_position);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(modturtle_heading_obj, modturtle_heading);

STATIC MP_DEFINE_CONST_FUN_OBJ_0(modturtle_pendown_obj, modturtle_pendown);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(modturtle_penup_obj, modturtle_penup);
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(modturtle_pensize_obj, 0, 1, modturtle_pensize);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(modturtle_isdown_obj, modturtle_isdown);

STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(modturtle_pencolor_obj, 0, 3, modturtle_pencolor);
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(modturtle_colormode_obj, 0, 1, modturtle_colormode);

STATIC MP_DEFINE_CONST_FUN_OBJ_0(modturtle_reset_obj, modturtle_reset);

STATIC MP_DEFINE_CONST_FUN_OBJ_0(modturtle_showturtle_obj, modturtle_showturtle);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(modturtle_hideturtle_obj, modturtle_hideturtle);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(modturtle_isvisible_obj, modturtle_isvisible);

STATIC MP_DEFINE_CONST_FUN_OBJ_0(modturtle___init___obj, modturtle___init__);

STATIC const mp_rom_map_elem_t modturtle_module_globals_table[] = {
  { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_turtle) },
  { MP_ROM_QSTR(MP_QSTR___init__), (mp_obj_t)&modturtle___init___obj },

  { MP_ROM_QSTR(MP_QSTR_forward), (mp_obj_t)&modturtle_forward_obj },
  { MP_ROM_QSTR(MP_QSTR_fd), (mp_obj_t)&modturtle_forward_obj },
  { MP_ROM_QSTR(MP_QSTR_backward), (mp_obj_t)&modturtle_backward_obj },
  { MP_ROM_QSTR(MP_QSTR_bk), (mp_obj_t)&modturtle_backward_obj },
  { MP_ROM_QSTR(MP_QSTR_back), (mp_obj_t)&modturtle_backward_obj },
  { MP_ROM_QSTR(MP_QSTR_right), (mp_obj_t)&modturtle_right_obj },
  { MP_ROM_QSTR(MP_QSTR_rt), (mp_obj_t)&modturtle_right_obj },
  { MP_ROM_QSTR(MP_QSTR_left), (mp_obj_t)&modturtle_left_obj },
  { MP_ROM_QSTR(MP_QSTR_lt), (mp_obj_t)&modturtle_left_obj },
  { MP_ROM_QSTR(MP_QSTR_goto), (mp_obj_t)&modturtle_goto_obj },
  { MP_ROM_QSTR(MP_QSTR_setpos), (mp_obj_t)&modturtle_goto_obj },
  { MP_ROM_QSTR(MP_QSTR_setposition), (mp_obj_t)&modturtle_goto_obj },
  { MP_ROM_QSTR(MP_QSTR_setheading), (mp_obj_t)&modturtle_setheading_obj },
  { MP_ROM_QSTR(MP_QSTR_seth), (mp_obj_t)&modturtle_setheading_obj },
  { MP_ROM_QSTR(MP_QSTR_circle), (mp_obj_t)&modturtle_circle_obj },
  { MP_ROM_QSTR(MP_QSTR_speed), (mp_obj_t)&modturtle_speed_obj },

  { MP_ROM_QSTR(MP_QSTR_position), (mp_obj_t)&modturtle_position_obj },
  { MP_ROM_QSTR(MP_QSTR_pos), (mp_obj_t)&modturtle_position_obj },
  { MP_ROM_QSTR(MP_QSTR_heading), (mp_obj_t)&modturtle_heading_obj },

  { MP_ROM_QSTR(MP_QSTR_pendown), (mp_obj_t)&modturtle_pendown_obj },
  { MP_ROM_QSTR(MP_QSTR_pd), (mp_obj_t)&modturtle_pendown_obj },
  { MP_ROM_QSTR(MP_QSTR_down), (mp_obj_t)&modturtle_pendown_obj },
  { MP_ROM_QSTR(MP_QSTR_penup), (mp_obj_t)&modturtle_penup_obj },
  { MP_ROM_QSTR(MP_QSTR_pu), (mp_obj_t)&modturtle_penup_obj },
  { MP_ROM_QSTR(MP_QSTR_up), (mp_obj_t)&modturtle_penup_obj },
  { MP_ROM_QSTR(MP_QSTR_pensize), (mp_obj_t)&modturtle_pensize_obj },
  { MP_ROM_QSTR(MP_QSTR_width), (mp_obj_t)&modturtle_pensize_obj },
  { MP_ROM_QSTR(MP_QSTR_isdown), (mp_obj_t)&modturtle_isdown_obj },

  { MP_ROM_QSTR(MP_QSTR_color), (mp_obj_t)&modturtle_pencolor_obj },
  { MP_ROM_QSTR(MP_QSTR_pencolor), (mp_obj_t)&modturtle_pencolor_obj },
  { MP_ROM_QSTR(MP_QSTR_colormode), (mp_obj_t)&modturtle_colormode_obj },

  { MP_ROM_QSTR(MP_QSTR_reset), (mp_obj_t)&modturtle_reset_obj },

  { MP_ROM_QSTR(MP_QSTR_showturtle), (mp_obj_t)&modturtle_showturtle_obj },
  { MP_ROM_QSTR(MP_QSTR_st), (mp_obj_t)&modturtle_showturtle_obj },
  { MP_ROM_QSTR(MP_QSTR_hideturtle), (mp_obj_t)&modturtle_hideturtle_obj },
  { MP_ROM_QSTR(MP_QSTR_ht), (mp_obj_t)&modturtle_hideturtle_obj },
  { MP_ROM_QSTR(MP_QSTR_isvisible), (mp_obj_t)&modturtle_isvisible_obj },
};

STATIC MP_DEFINE_CONST_DICT(modturtle_module_globals, modturtle_module_globals_table);

const mp_obj_module_t modturtle_module = {
  .base = { &mp_type_module },
  .globals = (mp_obj_dict_t*)&modturtle_module_globals,
};
