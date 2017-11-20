extern "C" {
#include "modkandinsky.h"
}
#include <escher/metric.h>
#include <kandinsky.h>
#include "port.h"

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
    KDPoint(mp_obj_get_int(x), mp_obj_get_int(y) + Metric::TitleBarHeight)
  );
  return MP_OBJ_NEW_SMALL_INT(c);
}
mp_obj_t kandinsky_set_pixel(mp_obj_t x, mp_obj_t y, mp_obj_t color) {
  KDIonContext::sharedContext()->setPixel(
    KDPoint(mp_obj_get_int(x), mp_obj_get_int(y) + Metric::TitleBarHeight),
    KDColor::RGB16(mp_obj_get_int(color))
  );
  MicroPython::ExecutionEnvironment::currentExecutionEnvironment()->didModifyFramebuffer();
  return mp_const_none;
}

mp_obj_t kandinsky_draw_string(mp_obj_t text, mp_obj_t x, mp_obj_t y) {
  KDIonContext::sharedContext()->drawString(
    mp_obj_str_get_str(text),
    KDPoint(mp_obj_get_int(x), mp_obj_get_int(y) + Metric::TitleBarHeight)
  );
  MicroPython::ExecutionEnvironment::currentExecutionEnvironment()->didModifyFramebuffer();
  return mp_const_none;
}

