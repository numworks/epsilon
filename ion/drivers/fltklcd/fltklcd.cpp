#include "fltklcd.h"
#include <stdlib.h>
#include <stdio.h> // FIXME: remove printfs

#include <FL/Fl_draw.H>

FltkLCD::FltkLCD(int x, int y, int w, int h) : Fl_Widget(x, y, w, h, NULL) {
  m_framebuffer = malloc(w*h);
// FIXME: Delete the framebuffer!
}

void FltkLCD::draw() {
  printf("DRAW\n");
  fl_draw_image_mono((const uchar *)m_framebuffer,
      0, // x
      0, // y
      w(), // width
      h(), // height,
      1, // byte-delta between pixels
      0); // byte-delta between lines;
}
