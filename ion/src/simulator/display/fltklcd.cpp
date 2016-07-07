#include "fltklcd.h"
#include <stdlib.h>
#include <FL/fl_draw.H>

FltkLCD::FltkLCD(int x, int y, int w, int h, KDColor * rgb565FrameBuffer) :
  Fl_Widget(x, y, w, h, nullptr),
  m_rgb565frameBufferStart(rgb565FrameBuffer),
  m_rgb565frameBufferEnd(rgb565FrameBuffer+w*h)
{
  m_rgb888frameBufferStart = malloc(w*h*3);
}

FltkLCD::~FltkLCD() {
  free(m_rgb888frameBufferStart);
}

void FltkLCD::draw() {
  // 1/ Convert the framebuffer from 565 to 888
  KDColor * rgb565Pixel = m_rgb565frameBufferStart;
  uint8_t * rgb888Component = (uint8_t *)m_rgb888frameBufferStart;

  while(rgb565Pixel < m_rgb565frameBufferEnd) {
    KDColor color = *rgb565Pixel++;
    *rgb888Component++ = color.red();
    *rgb888Component++ = color.green();
    *rgb888Component++ = color.blue();
  }

  // 2/ Draw the 888 framebuffer
  fl_draw_image((const uchar *)m_rgb888frameBufferStart,
      x(), // x
      y(), // y
      w(), // width
      h()); // height);
}
