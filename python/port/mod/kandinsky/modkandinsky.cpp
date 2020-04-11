extern "C" {
#include "modkandinsky.h"
#include <py/objtuple.h>
#include <py/runtime.h>
}
#include <kandinsky.h>
#include <escher/palette.h>
#include "port.h"

static KDColor ColorForObj(mp_obj_t obj) {

  if (MP_OBJ_IS_STR(obj)) {
    size_t l;
    KDColor color;
    if(Palette::ColorFromString(mp_obj_str_get_data(obj, &l), &color)){
      return color;
    } else {
      mp_raise_ValueError("This color couldn't be found");
      return KDColorBlack;
    }
  } else {
    size_t len;
    mp_obj_t * elem;

    mp_obj_get_array(obj, &len, &elem);
    if (len != 3) {
      mp_raise_TypeError("color needs 3 components");
    }

    return KDColor::RGB888(
      mp_obj_get_int(elem[0]),
      mp_obj_get_int(elem[1]),
      mp_obj_get_int(elem[2])
    );
  }
}

static mp_obj_t TupleForRGB(uint8_t r, uint8_t g, uint8_t b) {
  mp_obj_tuple_t * t = static_cast<mp_obj_tuple_t *>(MP_OBJ_TO_PTR(mp_obj_new_tuple(3, NULL)));
  t->items[0] = MP_OBJ_NEW_SMALL_INT(r);
  t->items[1] = MP_OBJ_NEW_SMALL_INT(g);
  t->items[2] = MP_OBJ_NEW_SMALL_INT(b);
  return MP_OBJ_FROM_PTR(t);
}

/* KDIonContext::sharedContext needs to be set to the wanted Rect before
 * calling kandinsky_get_pixel, kandinsky_set_pixel and kandinsky_draw_string.
 * We do this here with displaySandbox(), which pushes the SandboxController on
 * the stackViewController and forces the window to redraw itself.
 * KDIonContext::sharedContext is set to the frame of the last object drawn. */

mp_obj_t modkandinsky_color(mp_obj_t red, mp_obj_t green, mp_obj_t blue) {
  return TupleForRGB(
    mp_obj_get_int(red),
    mp_obj_get_int(green),
    mp_obj_get_int(blue)
  );
}

/* Calling ExecutionEnvironment::displaySandbox() hides the console and switches
 * to another mode. So it's a good idea to retrieve and handle input parameters
 * before calling displaySandbox, otherwise error messages (such as TypeError)
 * won't be visible until the user comes back to the console screen. */

mp_obj_t modkandinsky_get_pixel(mp_obj_t x, mp_obj_t y) {
  KDPoint point(mp_obj_get_int(x), mp_obj_get_int(y));
  KDColor c;
  KDIonContext::sharedContext()->getPixel(point, &c);
  return TupleForRGB(c.red(), c.green(), c.blue());
}

mp_obj_t modkandinsky_set_pixel(mp_obj_t x, mp_obj_t y, mp_obj_t color) {
  KDPoint point(mp_obj_get_int(x), mp_obj_get_int(y));
  KDColor kdColor = ColorForObj(color);
  MicroPython::ExecutionEnvironment::currentExecutionEnvironment()->displaySandbox();
  KDIonContext::sharedContext()->setPixel(point, kdColor);
  return mp_const_none;
}

mp_obj_t modkandinsky_draw_string(size_t n_args, const mp_obj_t * args) {
  const char * text = mp_obj_str_get_str(args[0]);
  KDPoint point(mp_obj_get_int(args[1]), mp_obj_get_int(args[2]));
  KDColor textColor = (n_args >= 4) ? ColorForObj(args[3]) : KDColorBlack;
  KDColor backgroundColor = (n_args >= 5) ? ColorForObj(args[4]) : KDColorWhite;
  MicroPython::ExecutionEnvironment::currentExecutionEnvironment()->displaySandbox();
  KDIonContext::sharedContext()->drawString(text, point, KDFont::LargeFont, textColor, backgroundColor);
  /* Before and after execution of "modkandinsky_draw_string",
   * "micropython_port_vm_hook_loop" is called by "mp_execute_bytecode" and will
   * call "micropython_port_interrupt_if_needed" every 20000 calls.
   * However, "drawString" function might take some time to execute leading to
   * drastically decrease the frequency of calls to
   * "micropython_port_vm_hook_loop" and thus to
   * "micropython_port_interrupt_if_needed". So we add an extra
   * check for user interruption here. This way the user can still interrupt an
   * infinite loop calling 'drawString' for instance. */
  micropython_port_interrupt_if_needed();
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
  KDColor color = ColorForObj(args[4]);

  MicroPython::ExecutionEnvironment::currentExecutionEnvironment()->displaySandbox();
  KDIonContext::sharedContext()->fillRect(rect, color);
  // Cf comment on modkandinsky_draw_string
  micropython_port_interrupt_if_needed();
  return mp_const_none;
}
