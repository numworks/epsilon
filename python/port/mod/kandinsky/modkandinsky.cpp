extern "C" {
#include "modkandinsky.h"
#include <py/objtuple.h>
#include <py/runtime.h>
}
#include <kandinsky.h>
#include "port.h"

static KDColor ColorForTuple(mp_obj_t tuple) {
    size_t len;
    mp_obj_t * elem;

    mp_obj_get_array(tuple, &len, &elem);
    if (len != 3) {
      mp_raise_TypeError("color needs 3 components");
    }

    return KDColor::RGB888(
      mp_obj_get_int(elem[0]),
      mp_obj_get_int(elem[1]),
      mp_obj_get_int(elem[2])
    );
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

mp_obj_t modkandinsky_get_pixel(mp_obj_t x, mp_obj_t y) {
  KDColor c = KDIonContext::sharedContext()->getPixel(
    KDPoint(mp_obj_get_int(x), mp_obj_get_int(y))
  );
  return TupleForRGB(c.red(), c.green(), c.blue());
}

mp_obj_t modkandinsky_set_pixel(mp_obj_t x, mp_obj_t y, mp_obj_t color) {
  MicroPython::ExecutionEnvironment::currentExecutionEnvironment()->displaySandbox();
  KDIonContext::sharedContext()->setPixel(
    KDPoint(mp_obj_get_int(x), mp_obj_get_int(y)),
    ColorForTuple(color)
  );
  return mp_const_none;
}

mp_obj_t modkandinsky_draw_string(mp_obj_t text, mp_obj_t x, mp_obj_t y) {
  MicroPython::ExecutionEnvironment::currentExecutionEnvironment()->displaySandbox();
  KDIonContext::sharedContext()->drawString(
    mp_obj_str_get_str(text),
    KDPoint(mp_obj_get_int(x), mp_obj_get_int(y))
  );
  return mp_const_none;
}

