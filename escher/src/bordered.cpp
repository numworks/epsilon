#include <escher/bordered.h>

namespace Escher {

void Bordered::drawBorderOfRect(KDContext* ctx, KDRect rect,
                                KDColor borderColor) const {
  KDCoordinate width = rect.width();
  KDCoordinate height = rect.height();
  // Draw rectangle around cell
  ctx->fillRect(KDRect(0, 0, width, k_separatorThickness), borderColor);
  ctx->fillRect(KDRect(0, k_separatorThickness, k_separatorThickness,
                       height - k_separatorThickness),
                borderColor);
  ctx->fillRect(KDRect(width - k_separatorThickness, k_separatorThickness,
                       k_separatorThickness, height - k_separatorThickness),
                borderColor);
  ctx->fillRect(
      KDRect(0, height - k_separatorThickness, width, k_separatorThickness),
      borderColor);
}

void Bordered::drawInnerRect(KDContext* ctx, KDRect rect,
                             KDColor backgroundColor) const {
  ctx->fillRect(KDRect(k_separatorThickness, k_separatorThickness,
                       rect.width() - 2 * k_separatorThickness,
                       rect.height() - 2 * k_separatorThickness),
                backgroundColor);
}

}  // namespace Escher