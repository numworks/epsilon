#include <kandinsky/context.h>
#include <assert.h>

KDContext::KDContext(KDPoint origin, KDRect clippingRect) :
  m_origin(origin),
  m_clippingRect(clippingRect)
{
}

void KDContext::setOrigin(KDPoint origin) {
  m_origin = origin;
}

void KDContext::setClippingRect(KDRect clippingRect) {
  m_clippingRect = clippingRect;
}
