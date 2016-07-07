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
