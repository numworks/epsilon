extern "C" {
#include "modion.h"
#include <py/objtuple.h>
#include <py/runtime.h>
}
#include <ion.h>
#include "apps/apps_container.h"
#include "apps/global_preferences.h"
#include "port.h"

mp_obj_t modion_keyboard_keydown(mp_obj_t key_o) {
  Ion::Keyboard::Key key = static_cast<Ion::Keyboard::Key>(mp_obj_get_int(key_o));
  Ion::Keyboard::State state = Ion::Keyboard::scan();
  micropython_port_interrupt_if_needed();
  return mp_obj_new_bool(state.keyDown(key));
}

mp_obj_t modion_battery() {
  return mp_obj_new_float(Ion::Battery::voltage());
}

mp_obj_t modion_battery_level(){
  return mp_obj_new_int(static_cast<int>(Ion::Battery::level()));
}

mp_obj_t modion_battery_ischarging(){
  return mp_obj_new_bool(Ion::Battery::isCharging());
}

struct key2mp
{
    Ion::Keyboard::Key key;
    mp_obj_t string;
};

const static key2mp keyMapping[] =
{
    { Ion::Keyboard::Key::Left, MP_ROM_QSTR(MP_QSTR_left) },
    { Ion::Keyboard::Key::Right, MP_ROM_QSTR(MP_QSTR_right) },
    { Ion::Keyboard::Key::Down, MP_ROM_QSTR(MP_QSTR_down) },
    { Ion::Keyboard::Key::Up, MP_ROM_QSTR(MP_QSTR_up) },
    { Ion::Keyboard::Key::OK, MP_ROM_QSTR(MP_QSTR_OK) },
    { Ion::Keyboard::Key::Back, MP_ROM_QSTR(MP_QSTR_back) },

    { Ion::Keyboard::Key::Home, MP_ROM_QSTR(MP_QSTR_home) },
    { Ion::Keyboard::Key::OnOff, MP_ROM_QSTR(MP_QSTR_onOff) },

    { Ion::Keyboard::Key::Shift, MP_ROM_QSTR(MP_QSTR_shift) },
    { Ion::Keyboard::Key::Alpha, MP_ROM_QSTR(MP_QSTR_alpha) },
    { Ion::Keyboard::Key::XNT, MP_ROM_QSTR(MP_QSTR_xnt) },
    { Ion::Keyboard::Key::Var, MP_ROM_QSTR(MP_QSTR_var) },
    { Ion::Keyboard::Key::Toolbox, MP_ROM_QSTR(MP_QSTR_toolbox) },
    { Ion::Keyboard::Key::Backspace, MP_ROM_QSTR(MP_QSTR_backspace) },

    { Ion::Keyboard::Key::Exp, MP_ROM_QSTR(MP_QSTR_exp) },
    { Ion::Keyboard::Key::Ln, MP_ROM_QSTR(MP_QSTR_ln) },
    { Ion::Keyboard::Key::Log, MP_ROM_QSTR(MP_QSTR_log) },
    { Ion::Keyboard::Key::Imaginary, MP_ROM_QSTR(MP_QSTR_imaginary) },
    { Ion::Keyboard::Key::Comma, MP_ROM_QSTR(MP_QSTR_comma) },
    { Ion::Keyboard::Key::Power, MP_ROM_QSTR(MP_QSTR_power) },

    { Ion::Keyboard::Key::Sine, MP_ROM_QSTR(MP_QSTR_sin) },
    { Ion::Keyboard::Key::Cosine, MP_ROM_QSTR(MP_QSTR_cos) },
    { Ion::Keyboard::Key::Tangent, MP_ROM_QSTR(MP_QSTR_tan) },
    { Ion::Keyboard::Key::Pi, MP_ROM_QSTR(MP_QSTR_pi) },
    { Ion::Keyboard::Key::Sqrt, MP_ROM_QSTR(MP_QSTR_sqrt) },
    { Ion::Keyboard::Key::Square, MP_ROM_QSTR(MP_QSTR_square) },

    { Ion::Keyboard::Key::Seven, MP_ROM_QSTR(MP_QSTR_7) },
    { Ion::Keyboard::Key::Eight, MP_ROM_QSTR(MP_QSTR_8) },
    { Ion::Keyboard::Key::Nine, MP_ROM_QSTR(MP_QSTR_9) },
    { Ion::Keyboard::Key::LeftParenthesis, MP_ROM_QSTR(MP_QSTR__paren_open_) },
    { Ion::Keyboard::Key::RightParenthesis, MP_ROM_QSTR(MP_QSTR__paren_close_) },

    { Ion::Keyboard::Key::Four, MP_ROM_QSTR(MP_QSTR_4) },
    { Ion::Keyboard::Key::Five, MP_ROM_QSTR(MP_QSTR_5) },
    { Ion::Keyboard::Key::Six, MP_ROM_QSTR(MP_QSTR_6) },
    { Ion::Keyboard::Key::Multiplication, MP_ROM_QSTR(MP_QSTR__star_) },
    { Ion::Keyboard::Key::Division, MP_ROM_QSTR(MP_QSTR__slash_) },

    { Ion::Keyboard::Key::One, MP_ROM_QSTR(MP_QSTR_1) },
    { Ion::Keyboard::Key::Two, MP_ROM_QSTR(MP_QSTR_2) },
    { Ion::Keyboard::Key::Three, MP_ROM_QSTR(MP_QSTR_3) },
    { Ion::Keyboard::Key::Plus, MP_ROM_QSTR(MP_QSTR__plus_) },
    { Ion::Keyboard::Key::Minus, MP_ROM_QSTR(MP_QSTR__hyphen_) },

    { Ion::Keyboard::Key::Zero, MP_ROM_QSTR(MP_QSTR_0) },
    { Ion::Keyboard::Key::Dot, MP_ROM_QSTR(MP_QSTR__dot_) },
    { Ion::Keyboard::Key::EE, MP_ROM_QSTR(MP_QSTR_EE) },
    { Ion::Keyboard::Key::Ans, MP_ROM_QSTR(MP_QSTR_Ans) },
    { Ion::Keyboard::Key::EXE, MP_ROM_QSTR(MP_QSTR_EXE) },
};

mp_obj_t modion_get_keys() {
  micropython_port_interrupt_if_needed();

  Ion::Keyboard::State keys = Ion::Keyboard::scan();
  mp_obj_t result = mp_obj_new_set(0, nullptr);

  for (unsigned i = 0; i < sizeof(keyMapping)/sizeof(key2mp); i++) {
      if (keys.keyDown(keyMapping[i].key)) {
          mp_obj_set_store(result, keyMapping[i].string);
      }
  }

  return result;
}

mp_obj_t modion_set_brightness(mp_obj_t brightness_mp){
  int brightness = mp_obj_get_int(brightness_mp);
  if (brightness < 0 || brightness > 240) {
    mp_raise_ValueError("Brightness must be between 0 and 240");
  } else {
    uint8_t unsignedBrightness = static_cast<uint8_t>(brightness);
    GlobalPreferences::sharedGlobalPreferences()->setBrightnessLevel(unsignedBrightness);
    Ion::Backlight::setBrightness(unsignedBrightness);
    micropython_port_interrupt_if_needed();
    return mp_const_none;
  }
}

mp_obj_t modion_get_brightness(){
  uint8_t brightness = GlobalPreferences::sharedGlobalPreferences()->brightnessLevel();
  micropython_port_interrupt_if_needed();
  return mp_obj_new_int((int)brightness);
}
