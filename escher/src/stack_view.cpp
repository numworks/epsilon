#include <escher/stack_view.h>
extern "C" {
#include <assert.h>
}

StackView::StackView() :
  ChildlessView(),
  m_name(nullptr)
{
}

void StackView::setName(const char * name) {
  m_name = name;
  markRectAsDirty(bounds());
}

void StackView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(rect, KDColor(0xFFCD50));
  ctx->drawString(m_name, KDPointZero);
}

#if ESCHER_VIEW_LOGGING
const char * StackView::className() const {
  return "StackView";
}

void StackView::logAttributes(std::ostream &os) const {
  View::logAttributes(os);
  os << " name=\"" << m_name << "\"";
}
#endif
