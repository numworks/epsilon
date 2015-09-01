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
  Fl_Window * window = new Fl_Window(360, 280);
#if 1
  FltkLCD * lcd = new FltkLCD(20, 20, 320, 240);
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
