extern "C" {
#include <ion.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <kandinsky.h>
#include "init.h"
}
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>

#include "display/fltklcd.h"
#include "keyboard/fltkkbd.h"

static FltkLCD * sDisplay;
static FltkKbd * sKeyboard;
static KDFrameBuffer sFrameBuffer;

#define FRAMEBUFFER_ADDRESS (sDisplay->m_framebuffer)

void init_platform() {
  Fl::visual(FL_RGB);

  int margin = 10;
  int screen_width = ION_SCREEN_WIDTH;
  int screen_height = ION_SCREEN_HEIGHT;
  int keyboard_height = screen_width;

  Fl_Window * window = new Fl_Window(screen_width+2*margin, margin+screen_height+margin+keyboard_height+margin);

  sFrameBuffer.pixels = (KDColor *)malloc(ION_SCREEN_WIDTH*ION_SCREEN_HEIGHT*2);
  sFrameBuffer.size.width = ION_SCREEN_WIDTH;
  sFrameBuffer.size.height = ION_SCREEN_HEIGHT;
  /*
  sFrameBuffer.drawingArea.origin = KDPointZero;
  sFrameBuffer.drawingArea.size = sFrameBuffer.size;
  sFrameBuffer.drawingCursor = KDPointZero;
  */

  sDisplay = new FltkLCD(margin, margin, screen_width, screen_height, sFrameBuffer.pixels);
  sKeyboard = new FltkKbd(margin, margin+screen_height+margin, screen_width, keyboard_height);

  window->end();
  window->show();

  //KDCurrentContext->fillRect = NULL;
}

void ion_screen_push_rect(KDRect rect, const KDColor * pixels) {
  KDFramePushRect(&sFrameBuffer, rect, pixels);
}

void ion_screen_push_rect_uniform(KDRect rect, KDColor color) {
  KDFramePushRectUniform(&sFrameBuffer, rect, color);
}

void ion_screen_pull_rect(KDRect rect, KDColor * pixels) {
  KDFramePullRect(&sFrameBuffer, rect, pixels);
}

bool ion_key_down(ion_key_t key) {
  return sKeyboard->key_down(key);
}

void ion_sleep() {
  usleep(1000);
  sDisplay->redraw();
  Fl::wait();
}
