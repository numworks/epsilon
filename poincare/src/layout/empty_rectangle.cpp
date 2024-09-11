#include "empty_rectangle.h"

namespace Poincare::Internal {

KDSize EmptyRectangle::Size(KDFont::Size font, bool withMargins) {
  return KDSize(
      KDFont::GlyphWidth(font) - (withMargins ? 0 : 2 * k_marginWidth),
      KDFont::GlyphHeight(font) - (withMargins ? 0 : 2 * k_marginHeight));
}

void EmptyRectangle::DrawEmptyRectangle(KDContext* ctx, KDPoint p,
                                        KDFont::Size font, Color rectangleColor,
                                        const LayoutStyle& style) {
  KDSize rectangleSize = Size(font, false);

  KDColor fillColor = rectangleColor == Color::Yellow ? style.emptySquareColor
                                                      : style.placeholderColor;
  ctx->fillRect(
      KDRect(p.x() + k_marginWidth, p.y() + k_marginHeight, rectangleSize),
      fillColor);
}

}  // namespace Poincare::Internal
