extern "C" {
#include <ion.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include "init.h"
}
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>

#include "display/fltklcd.h"
#include "keyboard/fltkkbd.h"

static FltkLCD * sDisplay;
static FltkKbd * sKeyboard;

#define FRAMEBUFFER_ADDRESS (sDisplay->m_framebuffer)
#define FRAMEBUFFER_WIDTH 320
#define FRAMEBUFFER_HEIGHT 240
#define FRAMEBUFFER_BITS_PER_PIXEL 8

void init_platform() {
  Fl::visual(FL_RGB);
  int margin = 10;

  Fl_Window * window = new Fl_Window(FRAMEBUFFER_WIDTH+2*margin, FRAMEBUFFER_HEIGHT+2*margin+120+2*margin);

  sDisplay = new FltkLCD(margin, margin, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT);
  assert(FRAMEBUFFER_BITS_PER_PIXEL == 8);

  sKeyboard = new FltkKbd(margin,FRAMEBUFFER_HEIGHT+3*margin,FRAMEBUFFER_WIDTH,120);

  window->end();
  window->show(NULL, NULL);
}

void ion_set_pixel(uint8_t x, uint8_t y, uint8_t color) {
  assert(x <= FRAMEBUFFER_WIDTH);
  assert(y <= FRAMEBUFFER_HEIGHT);
  char * byte = (char *)(FRAMEBUFFER_ADDRESS) + ((y*FRAMEBUFFER_WIDTH)+x);
  *byte = color;
}

bool ion_key_down(ion_key_t key) {
  return sKeyboard->key_down(key);
}

void ion_sleep() {
  usleep(1000);
  sDisplay->redraw();
  Fl::wait();
}
