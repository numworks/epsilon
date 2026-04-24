#include <escher/bordered.h>
#include <escher/theme/color_picker.h>

namespace Escher {

void Bordered::drawBorderOfRect(KDContext* ctx, KDRect rect,
                                ThemeComponent borderColor) const {
  KDColor color = ColorPicker::Resolve(borderColor);
  KDCoordinate width = rect.width();
  KDCoordinate height = rect.height();
  // Draw rectangle around cell
  ctx->fillRect(KDRect(0, 0, width, k_separatorThickness), color);
  ctx->fillRect(KDRect(0, k_separatorThickness, k_separatorThickness,
                       height - k_separatorThickness),
                color);
  ctx->fillRect(KDRect(width - k_separatorThickness, k_separatorThickness,
                       k_separatorThickness, height - k_separatorThickness),
                color);
  ctx->fillRect(
      KDRect(0, height - k_separatorThickness, width, k_separatorThickness),
      color);
}

void Bordered::drawInnerRect(KDContext* ctx, KDRect rect,
                             ThemeComponent backgroundColor) const {
  ctx->fillRect(KDRect(k_separatorThickness, k_separatorThickness,
                       rect.width() - 2 * k_separatorThickness,
                       rect.height() - 2 * k_separatorThickness),
                ColorPicker::Resolve(backgroundColor));
}

}  // namespace Escher