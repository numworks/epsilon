extern "C" {
#include <ion.h>
#include <assert.h>
#include <stdio.h>
}
#include "platform.h"
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <ion/drivers/fltklcd/fltklcd.h>

void ion_init() {
  Fl::visual(FL_RGB);
  int margin = 10;
  Fl_Window * window = new Fl_Window(ION_FRAMEBUFFER_WIDTH+2*margin, ION_FRAMEBUFFER_HEIGHT+2*margin);
#if 1
  FltkLCD * lcd = new FltkLCD(margin, margin, ION_FRAMEBUFFER_WIDTH, ION_FRAMEBUFFER_HEIGHT);
  assert(ION_FRAMEBUFFER_BITS_PER_PIXEL == 8);
  Platform.display = lcd;
  PlatformFramebuffer = lcd->m_framebuffer;
#else
  Fl_Box * box = new Fl_Box(20,20,320,240,"Hello, World!");
  box->box(FL_UP_BOX);
  box->labelfont(FL_BOLD+FL_ITALIC);
  box->labelsize(36);
  box->labeltype(FL_SHADOW_LABEL);
#endif
  window->end();
  window->show(NULL, NULL);
  //Fl::run();
}

void ion_sleep() {
  printf("ion_sleep\n");
  Platform.display->redraw();
  Fl::wait();
}
