#include <escher/text_view.h>

namespace Escher {

KDSize TextView::minimalSizeForOptimalDisplay() const {
  if (text() == nullptr) {
    return KDSizeZero;
  }
  return KDFont::Font(m_glyphFormat.style.font)->stringSize(text());
}

void TextView::drawRect(KDContext* ctx, KDRect rect) const {
  if (text() == nullptr) {
    return;
  }
  ctx->fillRect(bounds(), m_glyphFormat.style.backgroundColor);
  ctx->alignAndDrawString(text(), KDPointZero, bounds().size(), m_glyphFormat);
}

}  // namespace Escher
