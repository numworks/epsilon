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
  // copy framebuffer
  const short unsigned int * ptr = (const short unsigned int *) Ion::Simulator::Framebuffer::address();
  Gc * gcptr = get_gc();
  for (int j = 0; j < LCD_HEIGHT_PX; ++j) {
    for (int i = 0; i < LCD_WIDTH_PX;){
      int c = *ptr;
      int k = 1;
      for (; k+i < LCD_WIDTH_PX; ++k) {
      	if (ptr[k]!=c) {
	        break;
        }
      }
      gui_gc_setColor(*gcptr,c_rgb565to888(c));
      gui_gc_drawRect(*gcptr,i,j,k-1,0);
      ptr += k;
      i += k;
    }
  }
  sync_screen();
}

}
}
}
