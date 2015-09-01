#include <kandinsky/rect.h>
#include <kandinsky/pixel.h>
#include <string.h>

void KDFillRect(KDRect rect, KDColor color) {
  //for (KDCoordinate y = rect.y ; y < (rect.y + rect.height); y++) {
    //memset(KDPixelAddress((KDPoint){.x=rect.x, .y=y}), color, rect.width);
  //}
  KDPoint p;
  for (p.x = rect.x; p.x<(rect.x+rect.width); p.x++) {
    for (p.y = rect.y; p.y<(rect.y+rect.height); p.y++) {
      KDSetPixel(p, color);
    }
  }
}
