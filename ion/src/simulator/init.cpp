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
static KDFrameBuffer * sFrameBuffer;

#define FRAMEBUFFER_ADDRESS (sDisplay->m_framebuffer)

void init_platform() {
  Fl::visual(FL_RGB);

  int margin = 10;
  int screen_width = ION_SCREEN_WIDTH;
  int screen_height = ION_SCREEN_HEIGHT;
  int keyboard_height = screen_width;

  Fl_Window * window = new Fl_Window(screen_width+2*margin, margin+screen_height+margin+keyboard_height+margin);

  KDColor * pixels = (KDColor *)malloc(ION_SCREEN_WIDTH*ION_SCREEN_HEIGHT*sizeof(KDColor));
  sFrameBuffer = new KDFrameBuffer(pixels, KDSize(ION_SCREEN_WIDTH, ION_SCREEN_HEIGHT));
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

void ion_screen_push_rect(uint16_t x, uint16_t y,
    uint16_t width, uint16_t height,
    const ion_color_t * pixels)
{
  // FIXME: Boy those casts are fugly
  const void * foo = static_cast<const void *>(pixels);
  const KDColor * pouet = static_cast<const KDColor *>(foo);
  sFrameBuffer->pushRect(KDRect(x,y,width,height), pouet);
}

void ion_screen_push_rect_uniform(uint16_t x, uint16_t y,
    uint16_t width, uint16_t height,
    ion_color_t color)
{
  ion_color_t * foo = &color;
  const void * bar = static_cast<const void *>(foo);
  const KDColor * baz = static_cast<const KDColor *>(bar);
  sFrameBuffer->pushRectUniform(KDRect(x,y,width,height), *baz);
}

void ion_screen_pull_rect(uint16_t x, uint16_t y,
    uint16_t width, uint16_t height,
    ion_color_t * pixels)
{
  void * foo = static_cast<void *>(pixels);
  KDColor * pouet = static_cast<KDColor *>(foo);
  sFrameBuffer->pullRect(KDRect(x,y,width,height), pouet);
}

bool ion_key_down(ion_key_t key) {
  return sKeyboard->key_down(key);
}

void ion_sleep(long ms) {
  usleep(1000*ms);
  sDisplay->redraw();
  Fl::wait();
}
