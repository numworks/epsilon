extern "C" {
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
}
#include "init.h"
#include <ion.h>
#include <kandinsky.h>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>

#include "display/fltklcd.h"
#include "keyboard/fltkkbd.h"

static FltkLCD * sDisplay;
static FltkKbd * sKeyboard;
static KDFrameBuffer * sFrameBuffer;

#define FRAMEBUFFER_ADDRESS (sDisplay->m_framebuffer)

void init_platform() {
  Fl::visual(FL_RGB);

  int margin = 10;
  int screen_width = Ion::Display::Width;
  int screen_height = Ion::Display::Height;
  int keyboard_height = screen_width;

  Fl_Window * window = new Fl_Window(screen_width+2*margin, margin+screen_height+margin+keyboard_height+margin);

  KDColor * pixels = (KDColor *)malloc(Ion::Display::Width*Ion::Display::Height*sizeof(KDColor));
  sFrameBuffer = new KDFrameBuffer(pixels, KDSize(Ion::Display::Width, Ion::Display::Height));
  /*
  sFrameBuffer.drawingArea.origin = KDPointZero;
  sFrameBuffer.drawingArea.size = sFrameBuffer.size;
  sFrameBuffer.drawingCursor = KDPointZero;
  */

  sDisplay = new FltkLCD(margin, margin, screen_width, screen_height, pixels);
  sKeyboard = new FltkKbd(margin, margin+screen_height+margin, screen_width, keyboard_height);

  window->end();
  window->show();

  //KDCurrentContext->fillRect = NULL;
}

void Ion::Display::pushRect(KDRect r, const KDColor * pixels) {
  sFrameBuffer->pushRect(r, pixels);
}

void Ion::Display::pushRectUniform(KDRect r, KDColor c) {
  sFrameBuffer->pushRectUniform(r, c);
}

void Ion::Display::pullRect(KDRect r, KDColor * pixels) {
  sFrameBuffer->pullRect(r, pixels);
}

bool Ion::Keyboard::keyDown(Ion::Keyboard::Key key) {
  return sKeyboard->key_down(key);
}

void Ion::msleep(long ms) {
  usleep(1000*ms);
  sDisplay->redraw();
  Fl::wait();
}
