#include <escher/stack_view.h>
extern "C" {
#include <assert.h>
}

StackView::StackView() :
  View(),
  m_controller(nullptr)
{
}

void StackView::setTextColor(KDColor textColor) {
  m_textColor = textColor;
  markRectAsDirty(bounds());
}

void StackView::setBackgroundColor(KDColor backgroundColor) {
  m_backgroundColor = backgroundColor;
  markRectAsDirty(bounds());
}

void StackView::setSeparatorColor(KDColor separatorColor) {
  m_separatorColor = separatorColor;
  markRectAsDirty(bounds());
}

void StackView::setNamedController(ViewController * controller) {
  m_controller = controller;
  markRectAsDirty(bounds());
}

void StackView::drawRect(KDContext * ctx, KDRect rect) const {
  KDRect b = bounds();
  drawBorderOfRect(ctx, b, m_separatorColor);
  drawInnerRect(ctx, b, m_backgroundColor);
  // Write title
  const KDFont * font = KDFont::SmallFont;
  KDSize textSize = font->stringSize(m_controller->title());
  KDPoint origin((m_frame.width() - textSize.width())/2,(m_frame.height() - textSize.height())/2);
  ctx->drawString(m_controller->title(), origin, font, m_textColor, m_backgroundColor);
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
