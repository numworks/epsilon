#include "empty_rectangle.h"

namespace Poincare::Internal {

constexpr static KDCoordinate k_marginWidth = 1;

#if POINCARE_SCANDIUM_LAYOUTS
constexpr static KDCoordinate k_marginTop = 2;
constexpr static KDCoordinate k_marginBottom = 3;
#else
constexpr static KDCoordinate k_marginTop = 3;
constexpr static KDCoordinate k_marginBottom = 3;
#endif

KDSize EmptyRectangle::Size(KDFont::Size font, bool withMargins) {
  return KDSize(
#if POINCARE_SCANDIUM_LAYOUTS
      KDFont::GlyphWidth(font, '0') - (withMargins ? 0 : 2 * k_marginWidth),
#else
      KDFont::GlyphMaxWidth(font) - (withMargins ? 0 : 2 * k_marginWidth),
#endif
      KDFont::GlyphHeight(font) -
          (withMargins ? 0 : k_marginTop + k_marginBottom));
}

void EmptyRectangle::DrawEmptyRectangle(KDContext* ctx, KDPoint p,
                                        KDFont::Size font, KDColor fillColor) {
  KDSize rectangleSize = Size(font, false);

  ctx->fillRect(
      KDRect(p.x() + k_marginWidth, p.y() + k_marginTop, rectangleSize),
      fillColor);
}

}  // namespace Poincare::Internal
