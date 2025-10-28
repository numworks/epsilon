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

SolidColorWithBorderView::SolidColorWithBorderView(KDColor border,
                                                   KDColor color,
                                                   KDCoordinate borderWidth)
    : View(), m_color(color), m_border(border), m_borderWidth(borderWidth) {}

void SolidColorWithBorderView::setColors(KDColor color, KDColor border) {
  if (m_color != color || m_border != border) {
    m_color = color;
    m_border = border;
    markWholeFrameAsDirty();
  }
}

void SolidColorWithBorderView::drawRect(KDContext* ctx, KDRect rect) const {
  ctx->fillRect(bounds(), m_border);
  KDRect insideRect =
      KDRect(bounds().x() + m_borderWidth, bounds().y() + m_borderWidth,
             bounds().width() - 2 * m_borderWidth,
             bounds().height() - 2 * m_borderWidth);
  ctx->fillRect(insideRect, m_color);
}

#if ESCHER_VIEW_LOGGING
const char* SolidColorWithBorderView::className() const {
  return "SolidColorWithBorderView";
}

void SolidColorWithBorderView::logAttributes(std::ostream& os) const {
  View::logAttributes(os);
  os << " color=\"" << (int)m_color << '"';
  os << " borderColor=\"" << (int)m_border << '"';
  os << " width=\"" << m_borderWidth << '"';
}
#endif

}  // namespace Escher
