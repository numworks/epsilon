#include "py/obj.h"
#include "py/mphal.h"
#include "modturtle.h"

STATIC MP_DEFINE_CONST_FUN_OBJ_1(turtle_forward_obj, turtle_forward);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(turtle_backward_obj, turtle_backward);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(turtle_right_obj, turtle_right);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(turtle_left_obj, turtle_left);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(turtle_goto_obj, turtle_goto);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(turtle_setheading_obj, turtle_setheading);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(turtle_speed_obj, turtle_speed);

STATIC MP_DEFINE_CONST_FUN_OBJ_0(turtle_pendown_obj, turtle_pendown);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(turtle_penup_obj, turtle_penup);

STATIC MP_DEFINE_CONST_FUN_OBJ_3(turtle_color_obj, turtle_color);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(turtle___init___obj, turtle___init__);

STATIC const mp_rom_map_elem_t turtle_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_turtle) },
    { MP_ROM_QSTR(MP_QSTR___init__), (mp_obj_t)&turtle___init___obj },
    { MP_ROM_QSTR(MP_QSTR_forward), (mp_obj_t)&turtle_forward_obj },
    { MP_ROM_QSTR(MP_QSTR_backward), (mp_obj_t)&turtle_backward_obj },
    { MP_ROM_QSTR(MP_QSTR_right), (mp_obj_t)&turtle_right_obj },
    { MP_ROM_QSTR(MP_QSTR_left), (mp_obj_t)&turtle_left_obj },
    { MP_ROM_QSTR(MP_QSTR_goto), (mp_obj_t)&turtle_goto_obj },
    { MP_ROM_QSTR(MP_QSTR_setheading), (mp_obj_t)&turtle_setheading_obj },
    { MP_ROM_QSTR(MP_QSTR_speed), (mp_obj_t)&turtle_speed_obj },

    { MP_ROM_QSTR(MP_QSTR_pendown), (mp_obj_t)&turtle_pendown_obj },
    { MP_ROM_QSTR(MP_QSTR_penup), (mp_obj_t)&turtle_penup_obj },

    { MP_ROM_QSTR(MP_QSTR_color), (mp_obj_t)&turtle_color_obj },
};

STATIC MP_DEFINE_CONST_DICT(turtle_module_globals, turtle_module_globals_table);

const mp_obj_module_t turtle_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&turtle_module_globals,
};
