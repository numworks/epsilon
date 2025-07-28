#include <escher/solid_color_view.h>

namespace Escher {

SolidColorView::SolidColorView(KDColor color) : View(), m_color(color) {}

void SolidColorView::reload() { markWholeFrameAsDirty(); }

void SolidColorView::setColor(KDColor color) {
  if (m_color != color) {
    m_color = color;
    markWholeFrameAsDirty();
  }
}

void SolidColorView::drawRect(KDContext* ctx, KDRect rect) const {
  ctx->fillRect(rect, m_color);
}

#if ESCHER_VIEW_LOGGING
const char* SolidColorView::className() const { return "SolidColorView"; }

void SolidColorView::logAttributes(std::ostream& os) const {
  View::logAttributes(os);
  os << " color=\"" << (int)m_color << "\"";
}
#endif

}  // namespace Escher
