extern "C" {
#include <ion.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
}
#include "platform.h"
#include "framebuffer.h"
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <ion/drivers/fltklcd/fltklcd.h>
#include <ion/drivers/fltkkbd/fltkkbd.h>

void ion_init() {
  Fl::visual(FL_RGB);
  int margin = 10;

  Fl_Window * window = new Fl_Window(FRAMEBUFFER_WIDTH+2*margin, FRAMEBUFFER_HEIGHT+2*margin+120+2*margin);

  FltkLCD * lcd = new FltkLCD(margin, margin, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT);
  assert(FRAMEBUFFER_BITS_PER_PIXEL == 8);
  Platform.display = lcd;
  PlatformFramebuffer = lcd->m_framebuffer;

  FltkKbd * kbd = new FltkKbd(margin,FRAMEBUFFER_HEIGHT+3*margin,FRAMEBUFFER_WIDTH,120);
  Platform.keyboard = kbd;
  ion_key_states = kbd->m_keyStates;

  window->end();
  window->show(NULL, NULL);
  //Fl::run();
}

void ion_sleep() {
  usleep(1000);
  Platform.display->redraw();
  Fl::wait();
}
