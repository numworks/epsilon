extern "C" {
#include "modkandinsky.h"
#include <py/runtime.h>
}
#include <kandinsky.h>
#include <ion.h>
#include "port.h"


static mp_obj_t TupleForKDColor(KDColor c) {
  mp_obj_tuple_t * t = static_cast<mp_obj_tuple_t *>(MP_OBJ_TO_PTR(mp_obj_new_tuple(3, NULL)));
  t->items[0] = MP_OBJ_NEW_SMALL_INT(c.red());
  t->items[1] = MP_OBJ_NEW_SMALL_INT(c.green());
  t->items[2] = MP_OBJ_NEW_SMALL_INT(c.blue());
  return MP_OBJ_FROM_PTR(t);
}

/* KDIonContext::sharedContext needs to be set to the wanted Rect before
 * calling kandinsky_get_pixel, kandinsky_set_pixel and kandinsky_draw_string.
 * We do this here with displaySandbox(), which pushes the SandboxController on
 * the stackViewController and forces the window to redraw itself.
 * KDIonContext::sharedContext is set to the frame of the last object drawn. */

mp_obj_t modkandinsky_color(size_t n_args, const mp_obj_t *args) {
  mp_obj_t color;
  if (n_args == 1) {
    color = args[0];
  } else if (n_args == 2) {
    mp_raise_TypeError("color takes 1 or 3 arguments");
    return mp_const_none;
  } else {
    assert(n_args == 3);
    color = mp_obj_new_tuple(n_args, args);
  }
  return TupleForKDColor(MicroPython::Color::Parse(color));
}

/* Calling ExecutionEnvironment::displaySandbox() hides the console and switches
 * to another mode. So it's a good idea to retrieve and handle input parameters
 * before calling displaySandbox, otherwise error messages (such as TypeError)
 * won't be visible until the user comes back to the console screen. */

mp_obj_t modkandinsky_get_pixel(mp_obj_t x, mp_obj_t y) {
  KDPoint point(mp_obj_get_int(x), mp_obj_get_int(y));
  KDColor c;
  KDIonContext::sharedContext()->getPixel(point, &c);
  return TupleForKDColor(c);
}

mp_obj_t modkandinsky_set_pixel(mp_obj_t x, mp_obj_t y, mp_obj_t input) {
  KDPoint point(mp_obj_get_int(x), mp_obj_get_int(y));
  KDColor kdColor = MicroPython::Color::Parse(input);
  MicroPython::ExecutionEnvironment::currentExecutionEnvironment()->displaySandbox();
  KDIonContext::sharedContext()->setPixel(point, kdColor);
  return mp_const_none;
}

//TODO Use good colors
mp_obj_t modkandinsky_draw_string(size_t n_args, const mp_obj_t * args) {
  const char * text = mp_obj_str_get_str(args[0]);
  KDPoint point(mp_obj_get_int(args[1]), mp_obj_get_int(args[2]));
  KDColor textColor = (n_args >= 4) ? MicroPython::Color::Parse(args[3]) : KDColorBlack;
  KDColor backgroundColor = (n_args >= 5) ? MicroPython::Color::Parse(args[4]) : KDColorWhite;
  MicroPython::ExecutionEnvironment::currentExecutionEnvironment()->displaySandbox();
  KDIonContext::sharedContext()->drawString(text, point, KDFont::LargeFont, textColor, backgroundColor);
  return mp_const_none;
}

mp_obj_t modkandinsky_fill_rect(size_t n_args, const mp_obj_t * args) {
  mp_int_t x = mp_obj_get_int(args[0]);
  mp_int_t y = mp_obj_get_int(args[1]);
  mp_int_t width = mp_obj_get_int(args[2]);
  mp_int_t height = mp_obj_get_int(args[3]);
  if (width < 0) {
    width = -width;
    x = x - width;
  }
  if (height < 0) {
    height = -height;
    y = y - height;
  }
  KDRect rect(x, y, width, height);
  KDColor color = MicroPython::Color::Parse(args[4]);
  MicroPython::ExecutionEnvironment::currentExecutionEnvironment()->displaySandbox();
  KDIonContext::sharedContext()->fillRect(rect, color);
  return mp_const_none;
}

mp_obj_t modkandinsky_wait_vblank() {
  micropython_port_interrupt_if_needed();
  Ion::Display::waitForVBlank();
  return mp_const_none;
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
    { Ion::Keyboard::Key::RightParenthesis, MP_ROM_QSTR(MP_QSTR__paren_open_) },
    { Ion::Keyboard::Key::LeftParenthesis, MP_ROM_QSTR(MP_QSTR__paren_close_) },

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

mp_obj_t modkandinsky_get_keys() {
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
