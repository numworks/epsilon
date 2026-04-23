#include <escher/solid_color_view.h>
#include <escher/theme/color_picker.h>

namespace Escher {

SolidColorView::SolidColorView(ThemeComponent color) : View(), m_color(color) {}

void SolidColorView::reload() { markWholeFrameAsDirty(); }

void SolidColorView::setColor(ThemeComponent color) {
  if (m_color != color) {
    m_color = color;
    markWholeFrameAsDirty();
  }
}

void SolidColorView::drawRect(KDContext* ctx, KDRect rect) const {
  ctx->fillRect(rect, ColorPicker::Resolve(m_color));
}

#if ESCHER_VIEW_LOGGING
const char* SolidColorView::className() const { return "SolidColorView"; }

void SolidColorView::logAttributes(std::ostream& os) const {
  View::logAttributes(os);
  os << " color=\"" << (int)ColorPicker::Resolve(m_color) << "\"";
}
#endif

SolidColorWithBorderView::SolidColorWithBorderView(ThemeComponent color,
                                                   ThemeComponent border,
                                                   KDCoordinate borderWidth)
    : View(), m_color(color), m_border(border), m_borderWidth(borderWidth) {}

void SolidColorWithBorderView::setColors(ThemeComponent color,
                                         ThemeComponent border) {
  if (m_color != color || m_border != border) {
    m_color = color;
    m_border = border;
    markWholeFrameAsDirty();
  }
}

void SolidColorWithBorderView::drawRect(KDContext* ctx, KDRect rect) const {
  ctx->fillRect(bounds(), ColorPicker::Resolve(m_border));
  KDRect insideRect =
      KDRect(bounds().x() + m_borderWidth, bounds().y() + m_borderWidth,
             bounds().width() - 2 * m_borderWidth,
             bounds().height() - 2 * m_borderWidth);
  ctx->fillRect(insideRect, ColorPicker::Resolve(m_color));
}

#if ESCHER_VIEW_LOGGING
const char* SolidColorWithBorderView::className() const {
  return "SolidColorWithBorderView";
}

void SolidColorWithBorderView::logAttributes(std::ostream& os) const {
  View::logAttributes(os);
  os << " color=\"" << (int)ColorPicker::Resolve(m_color) << '"';
  os << " borderColor=\"" << (int)ColorPicker::Resolve(m_border) << '"';
  os << " width=\"" << m_borderWidth << '"';
}
#endif

}  // namespace Escher
