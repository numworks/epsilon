#include <poincare/empty_rectangle.h>
#include <escher/palette.h>

namespace Poincare {

KDSize EmptyRectangle::RectangleSize(KDFont::Size font, bool withMargins) {
  return KDSize(KDFont::GlyphWidth(font) - (withMargins ? 0 : 2 * k_marginWidth), KDFont::GlyphHeight(font) - (withMargins ? 0 : 2 * k_marginHeight));
}

void EmptyRectangle::DrawEmptyRectangle(KDContext * ctx, KDPoint p, KDFont::Size font, Color rectangleColor) {
  KDSize rectangleSize = RectangleSize(font, false);

  KDColor fillColor = rectangleColor == Color::Yellow ? Escher::Palette::YellowDark : Escher::Palette::GrayBright;
  ctx->fillRect(KDRect(p.x() + k_marginWidth, p.y() + k_marginHeight, rectangleSize.width(), rectangleSize.height()), fillColor);
}

}
