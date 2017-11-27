extern "C" {
#include "modkandinsky.h"
}
#include <kandinsky.h>
#include "port.h"

/* KDIonContext::sharedContext needs to be set to the wanted Rect before
 * calling kandinsky_get_pixel, kandinsky_set_pixel and kandinsky_draw_string.
 * We do this here with displaySandbox(), which pushes the SandboxController on
 * the stackViewController and forces the window to redraw itself.
 * KDIonContext::sharedContext is set to the frame of the last object drawn. */

mp_obj_t kandinsky_color(mp_obj_t red, mp_obj_t green, mp_obj_t blue) {
  return
    MP_OBJ_NEW_SMALL_INT(
      KDColor::RGB888(
        mp_obj_get_int(red),
        mp_obj_get_int(green),
        mp_obj_get_int(blue)
      )
    );
}

mp_obj_t kandinsky_get_pixel(mp_obj_t x, mp_obj_t y) {
  KDColor c = KDIonContext::sharedContext()->getPixel(
    KDPoint(mp_obj_get_int(x), mp_obj_get_int(y))
  );
  return MP_OBJ_NEW_SMALL_INT(c);
}

mp_obj_t kandinsky_set_pixel(mp_obj_t x, mp_obj_t y, mp_obj_t color) {
  MicroPython::ExecutionEnvironment::currentExecutionEnvironment()->displaySandbox();
  KDIonContext::sharedContext()->setPixel(
    KDPoint(mp_obj_get_int(x), mp_obj_get_int(y)),
    KDColor::RGB16(mp_obj_get_int(color))
  );
  return mp_const_none;
}

mp_obj_t kandinsky_draw_string(mp_obj_t text, mp_obj_t x, mp_obj_t y) {
  MicroPython::ExecutionEnvironment::currentExecutionEnvironment()->displaySandbox();
  KDIonContext::sharedContext()->drawString(
    mp_obj_str_get_str(text),
    KDPoint(mp_obj_get_int(x), mp_obj_get_int(y))
  );
  return mp_const_none;
}

