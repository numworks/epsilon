#include <escher/text_view.h>

namespace Escher {

KDSize TextView::minimalSizeForOptimalDisplay() const {
  if (text() == nullptr) {
    return KDSizeZero;
  }
  return KDFont::Font(m_font)->stringSize(text());
}

void TextView::drawRect(KDContext* ctx, KDRect rect) const {
  if (text() == nullptr) {
    return;
  }
  ctx->fillRect(bounds(), m_backgroundColor);
  ctx->alignAndDrawString(text(), KDPointZero, bounds().size(),
                          m_horizontalAlignment, m_verticalAlignment, m_font,
                          m_textColor, m_backgroundColor);
}

}  // namespace Escher
