extern "C" {
#include "modion.h"
}

#include <ion.h>

/* We cannot use C99-style struct initizalition in C++. As a result, we cannot
 * use the macros that micropython recommends, and we have to hand build those
 * structs. To avoid errors, we drop in a few static_asserts. */

static_assert(sizeof(mp_fun_1_t) == sizeof(mp_fun_0_t),
              "modion_table badly formed");
static_assert(sizeof(mp_obj_fun_builtin_fixed_t) ==
                  sizeof(mp_obj_base_t) + sizeof(mp_fun_1_t),
              "modion_keyboard_keydown_obj badly formed");

const mp_obj_fun_builtin_fixed_t modion_keyboard_keydown_obj = {
    {&mp_type_fun_builtin_1}, {(mp_fun_0_t)modion_keyboard_keydown}};

STATIC const mp_rom_map_elem_t modion_module_globals_table[] = {
    {MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_ion)},
    {MP_ROM_QSTR(MP_QSTR_keydown), MP_ROM_PTR(&modion_keyboard_keydown_obj)},
    {MP_ROM_QSTR(MP_QSTR_KEY_LEFT),
     MP_OBJ_NEW_SMALL_INT(Ion::Keyboard::Key::Left)},
    {MP_ROM_QSTR(MP_QSTR_KEY_UP), MP_OBJ_NEW_SMALL_INT(Ion::Keyboard::Key::Up)},
    {MP_ROM_QSTR(MP_QSTR_KEY_DOWN),
     MP_OBJ_NEW_SMALL_INT(Ion::Keyboard::Key::Down)},
    {MP_ROM_QSTR(MP_QSTR_KEY_RIGHT),
     MP_OBJ_NEW_SMALL_INT(Ion::Keyboard::Key::Right)},
    {MP_ROM_QSTR(MP_QSTR_KEY_OK), MP_OBJ_NEW_SMALL_INT(Ion::Keyboard::Key::OK)},
    {MP_ROM_QSTR(MP_QSTR_KEY_BACK),
     MP_OBJ_NEW_SMALL_INT(Ion::Keyboard::Key::Back)},
    {MP_ROM_QSTR(MP_QSTR_KEY_HOME),
     MP_OBJ_NEW_SMALL_INT(Ion::Keyboard::Key::Home)},
    {MP_ROM_QSTR(MP_QSTR_KEY_ONOFF),
     MP_OBJ_NEW_SMALL_INT(Ion::Keyboard::Key::OnOff)},
    {MP_ROM_QSTR(MP_QSTR_KEY_SHIFT),
     MP_OBJ_NEW_SMALL_INT(Ion::Keyboard::Key::Shift)},
    {MP_ROM_QSTR(MP_QSTR_KEY_ALPHA),
     MP_OBJ_NEW_SMALL_INT(Ion::Keyboard::Key::Alpha)},
    {MP_ROM_QSTR(MP_QSTR_KEY_XNT),
     MP_OBJ_NEW_SMALL_INT(Ion::Keyboard::Key::XNT)},
    {MP_ROM_QSTR(MP_QSTR_KEY_VAR),
     MP_OBJ_NEW_SMALL_INT(Ion::Keyboard::Key::Var)},
    {MP_ROM_QSTR(MP_QSTR_KEY_TOOLBOX),
     MP_OBJ_NEW_SMALL_INT(Ion::Keyboard::Key::Toolbox)},
    {MP_ROM_QSTR(MP_QSTR_KEY_BACKSPACE),
     MP_OBJ_NEW_SMALL_INT(Ion::Keyboard::Key::Backspace)},
    {MP_ROM_QSTR(MP_QSTR_KEY_EXP),
     MP_OBJ_NEW_SMALL_INT(Ion::Keyboard::Key::Exp)},
    {MP_ROM_QSTR(MP_QSTR_KEY_LN), MP_OBJ_NEW_SMALL_INT(Ion::Keyboard::Key::Ln)},
    {MP_ROM_QSTR(MP_QSTR_KEY_LOG),
     MP_OBJ_NEW_SMALL_INT(Ion::Keyboard::Key::Log)},
    {MP_ROM_QSTR(MP_QSTR_KEY_IMAGINARY),
     MP_OBJ_NEW_SMALL_INT(Ion::Keyboard::Key::Imaginary)},
    {MP_ROM_QSTR(MP_QSTR_KEY_COMMA),
     MP_OBJ_NEW_SMALL_INT(Ion::Keyboard::Key::Comma)},
    {MP_ROM_QSTR(MP_QSTR_KEY_POWER),
     MP_OBJ_NEW_SMALL_INT(Ion::Keyboard::Key::Power)},
    {MP_ROM_QSTR(MP_QSTR_KEY_SINE),
     MP_OBJ_NEW_SMALL_INT(Ion::Keyboard::Key::Sine)},
    {MP_ROM_QSTR(MP_QSTR_KEY_COSINE),
     MP_OBJ_NEW_SMALL_INT(Ion::Keyboard::Key::Cosine)},
    {MP_ROM_QSTR(MP_QSTR_KEY_TANGENT),
     MP_OBJ_NEW_SMALL_INT(Ion::Keyboard::Key::Tangent)},
    {MP_ROM_QSTR(MP_QSTR_KEY_PI), MP_OBJ_NEW_SMALL_INT(Ion::Keyboard::Key::Pi)},
    {MP_ROM_QSTR(MP_QSTR_KEY_SQRT),
     MP_OBJ_NEW_SMALL_INT(Ion::Keyboard::Key::Sqrt)},
    {MP_ROM_QSTR(MP_QSTR_KEY_SQUARE),
     MP_OBJ_NEW_SMALL_INT(Ion::Keyboard::Key::Square)},
    {MP_ROM_QSTR(MP_QSTR_KEY_SEVEN),
     MP_OBJ_NEW_SMALL_INT(Ion::Keyboard::Key::Seven)},
    {MP_ROM_QSTR(MP_QSTR_KEY_EIGHT),
     MP_OBJ_NEW_SMALL_INT(Ion::Keyboard::Key::Eight)},
    {MP_ROM_QSTR(MP_QSTR_KEY_NINE),
     MP_OBJ_NEW_SMALL_INT(Ion::Keyboard::Key::Nine)},
    {MP_ROM_QSTR(MP_QSTR_KEY_LEFTPARENTHESIS),
     MP_OBJ_NEW_SMALL_INT(Ion::Keyboard::Key::LeftParenthesis)},
    {MP_ROM_QSTR(MP_QSTR_KEY_RIGHTPARENTHESIS),
     MP_OBJ_NEW_SMALL_INT(Ion::Keyboard::Key::RightParenthesis)},
    {MP_ROM_QSTR(MP_QSTR_KEY_FOUR),
     MP_OBJ_NEW_SMALL_INT(Ion::Keyboard::Key::Four)},
    {MP_ROM_QSTR(MP_QSTR_KEY_FIVE),
     MP_OBJ_NEW_SMALL_INT(Ion::Keyboard::Key::Five)},
    {MP_ROM_QSTR(MP_QSTR_KEY_SIX),
     MP_OBJ_NEW_SMALL_INT(Ion::Keyboard::Key::Six)},
    {MP_ROM_QSTR(MP_QSTR_KEY_MULTIPLICATION),
     MP_OBJ_NEW_SMALL_INT(Ion::Keyboard::Key::Multiplication)},
    {MP_ROM_QSTR(MP_QSTR_KEY_DIVISION),
     MP_OBJ_NEW_SMALL_INT(Ion::Keyboard::Key::Division)},
    {MP_ROM_QSTR(MP_QSTR_KEY_ONE),
     MP_OBJ_NEW_SMALL_INT(Ion::Keyboard::Key::One)},
    {MP_ROM_QSTR(MP_QSTR_KEY_TWO),
     MP_OBJ_NEW_SMALL_INT(Ion::Keyboard::Key::Two)},
    {MP_ROM_QSTR(MP_QSTR_KEY_THREE),
     MP_OBJ_NEW_SMALL_INT(Ion::Keyboard::Key::Three)},
    {MP_ROM_QSTR(MP_QSTR_KEY_PLUS),
     MP_OBJ_NEW_SMALL_INT(Ion::Keyboard::Key::Plus)},
    {MP_ROM_QSTR(MP_QSTR_KEY_MINUS),
     MP_OBJ_NEW_SMALL_INT(Ion::Keyboard::Key::Minus)},
    {MP_ROM_QSTR(MP_QSTR_KEY_ZERO),
     MP_OBJ_NEW_SMALL_INT(Ion::Keyboard::Key::Zero)},
    {MP_ROM_QSTR(MP_QSTR_KEY_DOT),
     MP_OBJ_NEW_SMALL_INT(Ion::Keyboard::Key::Dot)},
    {MP_ROM_QSTR(MP_QSTR_KEY_EE), MP_OBJ_NEW_SMALL_INT(Ion::Keyboard::Key::EE)},
    {MP_ROM_QSTR(MP_QSTR_KEY_ANS),
     MP_OBJ_NEW_SMALL_INT(Ion::Keyboard::Key::Ans)},
    {MP_ROM_QSTR(MP_QSTR_KEY_EXE),
     MP_OBJ_NEW_SMALL_INT(Ion::Keyboard::Key::EXE)},
};

STATIC MP_DEFINE_CONST_DICT(modion_module_globals, modion_module_globals_table);

static_assert(sizeof(mp_obj_module_t) ==
                  sizeof(mp_obj_base_t) + sizeof(mp_obj_dict_t *),
              "modion_module badly formed");

extern "C" const mp_obj_module_t modion_module = {
    {&mp_type_module},
    (mp_obj_dict_t *)&modion_module_globals,
};
