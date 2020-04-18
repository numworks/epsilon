#include "display.h"
#include "framebuffer.h"
#include <assert.h>
#include <ion/display.h>
#include <string.h>

namespace Ion {
namespace Simulator {
namespace Display {

void* pixels;

void init() {
  gfxSetScreenFormat(GFX_TOP, GSP_BGR8_OES);
  pixels = nullptr;
}

void quit() {
  pixels = nullptr;
}

void draw() {
  pixels = gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL);
  
  
  for(int i = 0; i < Ion::Display::Width; i++) {
    for(int j = 0; j < Ion::Display::Height; j++) {
      ((u8*)pixels)[239*3 - j*3 + 0 + i*3*240 + 3*240*40] = Framebuffer::address()[i + j * Ion::Display::Width].blue();
      ((u8*)pixels)[239*3 - j*3 + 1 + i*3*240 + 3*240*40] = Framebuffer::address()[i + j * Ion::Display::Width].green();
      ((u8*)pixels)[239*3 - j*3 + 2 + i*3*240 + 3*240*40] = Framebuffer::address()[i + j * Ion::Display::Width].red();
    }
  }
  
	gfxFlushBuffers();
	gfxSwapBuffers();
}

}
}
}
