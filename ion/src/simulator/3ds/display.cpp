#include "display.h"
#include "framebuffer.h"
#include <assert.h>
#include <ion/display.h>
#include <string.h>

#include "keyboard.h"

namespace Ion {
namespace Simulator {
namespace Display {

void* pixels;

void init() {
  // gfxSetScreenFormat(GFX_TOP, GSP_BGR8_OES);
  //  gfxSetScreenFormat(GFX_BOTTOM, GSP_BGR8_OES);
  gfxSetDoubleBuffering(GFX_BOTTOM, false);
  pixels = nullptr;
}

void quit() {
  pixels = nullptr;
}

void draw() {
  pixels = gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL);
  
  
  for(int i = 0; i < Ion::Display::Width; i++) {
    for(int j = 0; j < Ion::Display::Height; j++) {
      u32 pixel = 239*3 - j*3 + i*3*240 + 3*240*40;
      ((u8*)pixels)[pixel + 0] = Framebuffer::address()[i + j * Ion::Display::Width].blue();
      ((u8*)pixels)[pixel + 1] = Framebuffer::address()[i + j * Ion::Display::Width].green();
      ((u8*)pixels)[pixel + 2] = Framebuffer::address()[i + j * Ion::Display::Width].red();
    }
  }
  
  u8* fb = gfxGetFramebuffer(GFX_BOTTOM, GFX_LEFT, NULL, NULL);
  memcpy(fb, keyboard_bgr, keyboard_bgr_len);
  
  gfxFlushBuffers();
  gfxSwapBuffers();
  
  gspWaitForVBlank();
}

}
}
}
