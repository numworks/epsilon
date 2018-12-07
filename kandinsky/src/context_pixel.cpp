#include <kandinsky/context.h>

void KDContext::setPixel(KDPoint p, KDColor c) {
  KDPoint absolutePoint = p.translatedBy(m_origin);
  if (m_clippingRect.contains(absolutePoint)) {
    pushRect(KDRect(absolutePoint, 1, 1), &c);
  }
}

KDColor KDContext::getPixel(KDPoint p) {
  KDPoint absolutePoint = p.translatedBy(m_origin);
  if (m_clippingRect.contains(absolutePoint)) {
    KDColor result = KDColorBlack;
    pullRect(KDRect(absolutePoint, 1, 1), &result);
    return result;
  }
  return KDColorBlack;
}

void KDContext::getPixels(KDRect r, KDColor * pixels) {
  KDRect rect = r.translatedBy(m_origin);
  if (m_clippingRect.containsRect(rect)) {
    pullRect(rect, pixels);
    return;
  }
  int i = 0;
  KDCoordinate yMax = r.y()+r.height();
  KDCoordinate xMax = r.x()+r.width();
  for (int y = r.y(); y < yMax; y++) {
    for (int x = r.x(); x < xMax; x++) {
      pixels[i++] = getPixel(KDPoint(x, y));
    }
  }
}
