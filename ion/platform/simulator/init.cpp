extern "C" {
#include <ion.h>
#include <assert.h>
#include <stdio.h>
}
#include "platform.h"
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <ion/drivers/fltklcd/fltklcd.h>
#include <ion/drivers/fltkkbd/fltkkbd.h>

void ion_init() {
  Fl::visual(FL_RGB);
  int margin = 10;
  Fl_Window * window = new Fl_Window(ION_FRAMEBUFFER_WIDTH+2*margin, ION_FRAMEBUFFER_HEIGHT+2*margin+40+2*margin);

  FltkLCD * lcd = new FltkLCD(margin, margin, ION_FRAMEBUFFER_WIDTH, ION_FRAMEBUFFER_HEIGHT);
  assert(ION_FRAMEBUFFER_BITS_PER_PIXEL == 8);
  Platform.display = lcd;
  PlatformFramebuffer = lcd->m_framebuffer;

  FltkKBD * kbd = new FltkKBD(0,0,100,100);
  Platform.keyboard = kbd;

  Fl_Button *button = new Fl_Button(margin, ION_FRAMEBUFFER_HEIGHT+2*margin+margin, 40, 40, "A");

  window->end();
  window->show(NULL, NULL);
  //Fl::run();
}

void ion_sleep() {
  printf("ion_sleep\n");
  Platform.display->redraw();
  Fl::wait();
}
