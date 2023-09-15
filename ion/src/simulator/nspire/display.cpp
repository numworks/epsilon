#include "display.h"
#include "framebuffer.h"
#include <kandinsky/color.h>
#include <stdint.h>
#include <ion/display.h>
#include <string.h>

#include <stdio.h>

#include "k_csdk.h"
#include <libndls.h>

namespace Ion {
namespace Simulator {
namespace Display {

void init() {
}

void quit() {
}

void draw() {
  unsigned short * ionFramebuffer = (unsigned short *) Ion::Simulator::Framebuffer::address();
  // we specify the screen fmt here because the "native" fmt varies between calculator revisions
  // some default to a 240x320 specification, which results in a 90-degree framebuffer rotation and other terribleness.
  lcd_blit(ionFramebuffer,SCR_320x240_565);
}

}
}
}
