#include <escher/bordered_cell.h>
#include <escher/palette.h>

void BorderedCell::drawRect(KDContext * ctx, KDRect rect) const {
  KDCoordinate width = bounds().width();
  KDCoordinate height = bounds().height();
  // Draw rectangle around cell
  ctx->fillRect(KDRect(0, 0, width, k_separatorThickness), Palette::GreyBright);
  ctx->fillRect(KDRect(0, k_separatorThickness, k_separatorThickness, height-k_separatorThickness), Palette::GreyBright);
  ctx->fillRect(KDRect(width-k_separatorThickness, k_separatorThickness, k_separatorThickness, height-k_separatorThickness), Palette::GreyBright);
  ctx->fillRect(KDRect(0, height-k_separatorThickness, width, k_separatorThickness), Palette::GreyBright);
 }
