extern "C" {
#include "modkandinsky.h"

#include <py/runtime.h>
}
#include <kandinsky/ion_context.h>

#include "port.h"

static mp_obj_t TupleForKDColor(KDColor c) {
  mp_obj_tuple_t *t =
      static_cast<mp_obj_tuple_t *>(MP_OBJ_TO_PTR(mp_obj_new_tuple(3, NULL)));
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
  KDIonContext::SharedContext->getPixel(point, &c);
  return TupleForKDColor(c);
}

mp_obj_t modkandinsky_set_pixel(mp_obj_t x, mp_obj_t y, mp_obj_t input) {
  KDPoint point(mp_obj_get_int(x), mp_obj_get_int(y));
  KDColor kdColor = MicroPython::Color::Parse(input);
  MicroPython::ExecutionEnvironment::currentExecutionEnvironment()
      ->displaySandbox();
  KDIonContext::SharedContext->setPixel(point, kdColor);
  return mp_const_none;
}

// TODO Use good colors
mp_obj_t modkandinsky_draw_string(size_t n_args, const mp_obj_t *args) {
  const char *text = mp_obj_str_get_str(args[0]);
  KDPoint point(mp_obj_get_int(args[1]), mp_obj_get_int(args[2]));
  KDColor textColor =
      (n_args >= 4) ? MicroPython::Color::Parse(args[3]) : KDColorBlack;
  KDColor backgroundColor =
      (n_args >= 5) ? MicroPython::Color::Parse(args[4]) : KDColorWhite;
  MicroPython::ExecutionEnvironment::currentExecutionEnvironment()
      ->displaySandbox();
  KDIonContext::SharedContext->drawString(
      text, point,
      KDGlyph::Style{.glyphColor = textColor,
                     .backgroundColor = backgroundColor,
                     .font = KDFont::Size::Large});
  return mp_const_none;
}

mp_obj_t modkandinsky_fill_rect(size_t n_args, const mp_obj_t *args) {
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
  MicroPython::ExecutionEnvironment::currentExecutionEnvironment()
      ->displaySandbox();
  KDIonContext::SharedContext->fillRect(rect, color);
  return mp_const_none;
}
