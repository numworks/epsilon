#include <apps/global_preferences.h>
#include <escher/solid_color_view.h>
#include <escher/theme/color_picker.h>

namespace Escher {

template <typename ColorType>
SolidColorViewBase<ColorType>::SolidColorViewBase(ColorType color)
    : View(), m_color(color) {}

template <typename ColorType>
void SolidColorViewBase<ColorType>::reload() {
  markWholeFrameAsDirty();
}

template <typename ColorType>
void SolidColorViewBase<ColorType>::setColor(ColorType color) {
  if (m_color != color) {
    m_color = color;
    markWholeFrameAsDirty();
  }
}

template <typename ColorType>
void SolidColorViewBase<ColorType>::drawRect(KDContext* ctx,
                                             KDRect rect) const {
  ctx->fillRect(rect, ColorPicker::Resolve(m_color));
}

#if ESCHER_VIEW_LOGGING
template <typename ColorType>
const char* SolidColorViewBase<ColorType>::className() const {
  return "SolidColorView";
}

template <typename ColorType>
void SolidColorViewBase<ColorType>::logAttributes(std::ostream& os) const {
  View::logAttributes(os);
  os << " color=\"" << (int)ColorPicker::Resolve(m_color) << "\"";
}
#endif

template <typename ColorType>
SolidColorWithBorderViewBase<ColorType>::SolidColorWithBorderViewBase(
    ColorType color, ColorType border, KDCoordinate borderWidth)
    : View(), m_color(color), m_border(border), m_borderWidth(borderWidth) {}

template <typename ColorType>
void SolidColorWithBorderViewBase<ColorType>::setColors(ColorType color,
                                                        ColorType border) {
  if (m_color != color || m_border != border) {
    m_color = color;
    m_border = border;
    markWholeFrameAsDirty();
  }
}

template <typename ColorType>
void SolidColorWithBorderViewBase<ColorType>::drawRect(KDContext* ctx,
                                                       KDRect rect) const {
  ctx->fillRect(bounds(), ColorPicker::Resolve(m_border));
  KDRect insideRect =
      KDRect(bounds().x() + m_borderWidth, bounds().y() + m_borderWidth,
             bounds().width() - 2 * m_borderWidth,
             bounds().height() - 2 * m_borderWidth);
  ctx->fillRect(insideRect, ColorPicker::Resolve(m_color));
}

#if ESCHER_VIEW_LOGGING
template <typename ColorType>
const char* SolidColorWithBorderViewBase<ColorType>::className() const {
  return "SolidColorWithBorderView";
}

template <typename ColorType>
void SolidColorWithBorderViewBase<ColorType>::logAttributes(
    std::ostream& os) const {
  View::logAttributes(os);
  os << " color=\"" << (int)ColorPicker::Resolve(m_color) << '"';
  os << " borderColor=\"" << (int)ColorPicker::Resolve(m_border) << '"';
  os << " width=\"" << m_borderWidth << '"';
}
#endif

// Explicit instantiations
template class SolidColorViewBase<ThemeComponent>;
template class SolidColorViewBase<KDColor>;
template class SolidColorWithBorderViewBase<ThemeComponent>;
template class SolidColorWithBorderViewBase<KDColor>;

}  // namespace Escher
