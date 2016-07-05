#include <kandinsky/context.h>
#include <ion.h>

KDContext KDIonContext = {
  .setPixel = &ion_set_pixel,
  .fillRect = &ion_fill_rect,
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
