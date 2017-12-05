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

#if defined(_WIN32) || defined(_WIN64)
  Fl::set_font(FL_SYMBOL, "Arial Unicode MS");
#endif

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

void Ion::Display::waitForVBlank() {
}

Ion::Keyboard::State Ion::Keyboard::scan() {
  Ion::Keyboard::State result = 0;
  for (int i=0; i<Ion::Keyboard::NumberOfKeys; i++) {
    result = result << 1 | sKeyboard->key_down((Ion::Keyboard::Key)(Ion::Keyboard::NumberOfKeys-1-i));

  }
  return result;
}

#include <chrono>

void Ion::msleep(long ms) {
  auto start = std::chrono::high_resolution_clock::now();
  while (true) {
    sDisplay->redraw();
    Fl::wait(0);
    auto elapsed = std::chrono::high_resolution_clock::now() - start;
    long long milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
    if (milliseconds >= ms) {
      break;
    }
  }
}

const char * Ion::serialNumber() {
  return "Simulator";
}
