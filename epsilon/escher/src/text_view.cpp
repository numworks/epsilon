#include <escher/text_view.h>

namespace Escher {

KDSize TextView::minimalSizeForOptimalDisplay() const {
  return KDFont::Font(m_glyphFormat.style.font)
      ->stringSize(text(), -1, m_lineSpacing);
}

void TextView::drawRect(KDContext* ctx, KDRect rect) const {
  if (text() == nullptr) {
    return;
  }
  ctx->fillRect(bounds(), m_glyphFormat.style.backgroundColor);
  ctx->alignAndDrawString(text(), KDPointZero, bounds().size(), m_glyphFormat,
                          -1, m_lineSpacing);
}

}  // namespace Escher
