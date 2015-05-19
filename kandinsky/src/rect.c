#include <kandinsky/rect.h>
#include <framebuffer.h>
#include <string.h>

void KDFillRect(KDRect rect, KDColor color) {
  for (KDCoordinate y = rect.y ; y < (rect.y + rect.height); y++) {
    memset(PIXEL_ADDRESS(rect.x, y), color, rect.width);
  }
}
