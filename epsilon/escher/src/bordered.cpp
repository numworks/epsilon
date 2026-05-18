#include <assert.h>
#include <escher/bordered.h>
#include <escher/theme/color_picker.h>

namespace Escher {

void Bordered::drawTopBorderOfRect(KDContext* ctx, KDRect rect,
                                   ThemeComponent borderColor) const {
  DrawInnerBorderOfRect(
      ctx, rect, k_separatorThickness, ColorPicker::Resolve(borderColor),
      Mask{.top = true, .right = false, .bottom = false, .left = false});
}

void Bordered::DrawBorderOfRect(KDContext* ctx, KDRect rect,
                                ThemeComponent borderColor) {
  DrawInnerBorderOfRect(ctx, rect, k_separatorThickness,
                        ColorPicker::Resolve(borderColor));
}

void Bordered::drawInnerRect(KDContext* ctx, KDRect rect,
                             ThemeComponent backgroundColor) const {
  assert(rect.origin() == KDPointZero);
  ctx->fillRect(KDRect(k_separatorThickness, k_separatorThickness,
                       rect.width() - 2 * k_separatorThickness,
                       rect.height() - 2 * k_separatorThickness),
                ColorPicker::Resolve(backgroundColor));
}

void Bordered::DrawOuterBorderOfRect(KDContext* ctx, KDRect rect,
                                     KDCoordinate size, KDColor color,
                                     Mask mask) {
  KDRect newRect = KDRect(rect.x() - size, rect.y() - size,
                          rect.width() + 2 * size, rect.height() + 2 * size);
  // Do not draw corners adjacents to a single border
  return PrivateDrawInnerBorderOfRect(ctx, newRect, size, color, mask, false);
}

void Bordered::DrawInnerBorderOfRect(KDContext* ctx, KDRect rect,
                                     KDCoordinate size, KDColor color,
                                     Mask mask) {
  // Draw corners adjacents to a single border
  return PrivateDrawInnerBorderOfRect(ctx, rect, size, color, mask, true);
}

void Bordered::PrivateDrawInnerBorderOfRect(KDContext* ctx, KDRect rect,
                                            KDCoordinate size, KDColor color,
                                            const Mask mask,
                                            bool drawAllCorners) {
  /* Note : This code is optimized to work with simple masks. Otherwise,
   * additional code to better handle corners may be needed. */
  assert(mask.left == mask.right);
  assert(drawAllCorners || (mask.left && mask.right));
  // Note : corners may be drawn twice with drawAllCorners and most masks
  KDCoordinate sideBorderOffset = drawAllCorners ? 0 : size;
  KDRect borders[4] = {
      // Top border with left and right corners
      mask.top ? KDRect(rect.left(), rect.top(), rect.width(), size)
               : KDRectZero,
      // Bottom border with left and right corners
      mask.bottom ? KDRect(rect.left(), rect.top() + rect.height() - size,
                           rect.width(), size)
                  : KDRectZero,
      // Left border (with top and bottom corners if drawAllCorners)
      mask.left ? KDRect(rect.left(), rect.top() + sideBorderOffset, size,
                         rect.height() - 2 * sideBorderOffset)
                : KDRectZero,
      // Right border (with top and bottom corners if drawAllCorners)
      mask.right ? KDRect(rect.left() + rect.width() - size,
                          rect.top() + sideBorderOffset, size,
                          rect.height() - 2 * sideBorderOffset)
                 : KDRectZero,
  };
  for (KDRect r : borders) {
    // fillRect will escape if rect is empty
    ctx->fillRect(r.intersectedWith(rect), color);
  }
}

}  // namespace Escher
