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
  KDCoordinate height = bounds().height();
  KDCoordinate width = bounds().width();
  ctx->fillRect(KDRect(0, 0, width, 1), m_separatorColor);
  ctx->fillRect(KDRect(0, 1, width, height-2), m_backgroundColor);
  ctx->fillRect(KDRect(0, height-1, width, 1), m_separatorColor);
  // Write title
  KDSize textSize = KDText::stringSize(m_controller->title(), KDText::FontSize::Small);
  KDPoint origin(0.5f*(m_frame.width() - textSize.width()),0.5f*(m_frame.height() - textSize.height()));
  ctx->drawString(m_controller->title(), origin, KDText::FontSize::Small, m_textColor, m_backgroundColor);
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
