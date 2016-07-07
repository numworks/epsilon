#include <kandinsky/context.h>
#include <ion.h>

KDContext KDIonContext = {
  .io = {
    .pushRect = &ion_screen_push_rect,
    .pushRectUniform = &ion_screen_push_rect_uniform,
    .pullRect = &ion_screen_pull_rect
  },
  .origin = {
    .x = 0,
    .y = 0
  },
  .clippingRect = {
    .x = 0,
    .y = 0,
    .width = ION_SCREEN_WIDTH,
    .height = ION_SCREEN_HEIGHT
  }
};

KDContext * KDCurrentContext = &KDIonContext;
