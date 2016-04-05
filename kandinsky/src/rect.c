#include <kandinsky/rect.h>
#include <kandinsky/pixel.h>
#include <string.h>

KDRect KDRectZero = {.x = 0, .y = 0, .width = 0, .height = 0};

void KDFillRect(KDRect rect, KDColor color) {
  KDPoint p;
  for (p.x = rect.x; p.x<(rect.x+rect.width); p.x++) {
    for (p.y = rect.y; p.y<(rect.y+rect.height); p.y++) {
      KDSetPixel(p, color);
    }
  }
}

void KDDrawRect(KDRect rect, KDColor color) {
  KDPoint p1, p2;
  p1.x = rect.x;
  p1.y = rect.y;
  p2.x = rect.x;
  p2.y = rect.y + rect.height;
  for (int i = 0; i<rect.width; i++) {
    KDSetPixel(p1, color);
    KDSetPixel(p2, color);
    p1.x++;
    p2.x++;
  }
  p1.x = rect.x;
  p1.y = rect.y;
  p2.x = rect.x + rect.width;
  p2.y = rect.y;
  for (int i = 0; i<rect.height; i++) {
    KDSetPixel(p1, color);
    KDSetPixel(p2, color);
    p1.y++;
    p2.y++;
  }
}
