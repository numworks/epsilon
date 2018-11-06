extern "C" {
#include "mod_turtle.h"
}
#include <kandinsky.h>
#include <ion.h>
#include <math.h>
#include "port.h"

#include "turtle_icon.h"

constexpr KDCoordinate TURTLE_SIZE = 9;
constexpr KDSize TURTLE_KDSIZE(TURTLE_SIZE, TURTLE_SIZE);
constexpr KDCoordinate X_OFFSET = 320 / 2;
constexpr KDCoordinate Y_OFFSET = (240-18) / 2;
constexpr KDCoordinate TURTLE_OFFSET = TURTLE_SIZE / 2;

static uint8_t s_mask[10*10];
static KDColor s_underTurtle[TURTLE_SIZE*TURTLE_SIZE];
static int s_curSize;

mp_obj_t mod_turtle_stamp(size_t n_args, const mp_obj_t *args) {
  int x = mp_obj_get_int(args[0]);
  int y = mp_obj_get_int(args[1]);
  int size = mp_obj_get_int(args[2]);
  KDColor color = KDColor::RGB16(mp_obj_get_int(args[3]));

  if (s_curSize != size) {
    /* Recompute the mask */
    const mp_float_t middle = size / 2;
    for (int j = 0; j < size; j++) {
      for (int i = 0; i < size; i++) {
        mp_float_t distance = sqrt((j - middle)*(j - middle) + (i - middle)*(i - middle)) / (middle+1);
        int value = distance * distance * 255;
        if (value < 0) {
          value = 0;
        }
        else if (value > 255) {
          value = 255;
        }
        s_mask[j*size + i] = value;
      }
    }
    s_curSize = size;
  }

  KDColor workingBuffer[size*size];
  MicroPython::ExecutionEnvironment::currentExecutionEnvironment()->displaySandbox();
  KDIonContext::sharedContext()->blendRectWithMask(KDRect(x+X_OFFSET, y+Y_OFFSET, KDSize(size, size)), color, s_mask, workingBuffer);

  return mp_const_none;
}

mp_obj_t mod_turtle_draw_turtle(mp_obj_t p_x, mp_obj_t p_y, mp_obj_t p_side) {
  int x = mp_obj_get_int(p_x);
  int y = mp_obj_get_int(p_y);
  int side = mp_obj_get_int(p_side);

  KDColor pixelBuffer[TURTLE_SIZE*TURTLE_SIZE*8];
  Ion::decompress(
    ImageStore::TurtleIcon->compressedPixelData(),
    reinterpret_cast<uint8_t *>(pixelBuffer),
    ImageStore::TurtleIcon->compressedPixelDataSize(),
    TURTLE_SIZE*TURTLE_SIZE*8 * sizeof(KDColor)
  );

  MicroPython::ExecutionEnvironment::currentExecutionEnvironment()->displaySandbox();
  KDIonContext::sharedContext()->fetchRect(KDRect(x+X_OFFSET-TURTLE_OFFSET, y+Y_OFFSET-TURTLE_OFFSET, TURTLE_KDSIZE), s_underTurtle);
  KDIonContext::sharedContext()->fillRectWithPixels(KDRect(x+X_OFFSET-TURTLE_OFFSET, y+Y_OFFSET-TURTLE_OFFSET, TURTLE_KDSIZE), pixelBuffer+(TURTLE_SIZE*TURTLE_SIZE*side), nullptr);

  return mp_const_none;
}

mp_obj_t mod_turtle_erase_turtle(mp_obj_t p_x, mp_obj_t p_y) {
  int x = mp_obj_get_int(p_x);
  int y = mp_obj_get_int(p_y);

  MicroPython::ExecutionEnvironment::currentExecutionEnvironment()->displaySandbox();
  KDIonContext::sharedContext()->fillRectWithPixels(KDRect(x+X_OFFSET-TURTLE_OFFSET, y+Y_OFFSET-TURTLE_OFFSET, TURTLE_KDSIZE), s_underTurtle, nullptr);

  return mp_const_none;
}
