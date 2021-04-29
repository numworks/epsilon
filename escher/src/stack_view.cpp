#include <escher/stack_view.h>
#include <escher/metric.h>
extern "C" {
#include <assert.h>
}

namespace Escher {

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
  // Add horizontal margins
  KDPoint point = KDPoint(Metric::CellLeftMargin, 0);
  KDSize size = KDSize(m_frame.width() - Metric::CellLeftMargin - Metric::CellRightMargin, m_frame.height());
  ctx->alignAndDrawString(m_controller->title(), point, size, 0.5f, 0.5f, font, m_textColor, m_backgroundColor);
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

}
