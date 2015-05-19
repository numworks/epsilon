#include <kandinsky/rect.h>
#include <kandinsky/referential.h>
#include <string.h>

void KDFillRect(KDRect rect, KDColor color) {
  for (KDCoordinate y = rect.y ; y < (rect.y + rect.height); y++) {
    memset(KDPixelAddress((KDPoint){.x=rect.x, .y=y}), color, rect.width);
  }
}
