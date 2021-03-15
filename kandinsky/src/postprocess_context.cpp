#include <kandinsky/postprocess_context.h>
#include <ion.h>

KDPostProcessContext::KDPostProcessContext() :
KDContext(KDPointZero, KDRectZero),
m_target(nullptr)
{
}

void KDPostProcessContext::setOrigin(KDPoint origin) {
  KDContext::setOrigin(origin);
  assert(m_target);
  m_target->setOrigin(origin);
}

void KDPostProcessContext::setClippingRect(KDRect clippingRect) {
  KDContext::setClippingRect(clippingRect);
  assert(m_target);
  m_target->setClippingRect(clippingRect);
}

void KDPostProcessContext::setTarget(KDContext * target)
{
  m_target = target;
}

void KDPostProcessContext::pushRect(KDRect rect, const KDColor * pixels)
{
  m_target->pushRect(rect, pixels);
}

void KDPostProcessContext::pushRectUniform(KDRect rect, KDColor color)
{
  m_target->pushRectUniform(rect, color);
}

void KDPostProcessContext::pullRect(KDRect rect, KDColor * pixels)
{
  m_target->pullRect(rect, pixels);
}
