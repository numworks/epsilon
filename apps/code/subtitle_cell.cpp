#include "subtitle_cell.h"

#include <assert.h>
#include <escher/palette.h>

using namespace Escher;

namespace Code {

SubtitleCell::SubtitleCell()
    : Bordered(),
      HighlightCell(),
      m_textView({.style = {.glyphColor = k_textColor,
                            .backgroundColor = k_backgroundColor,
                            .font = KDFont::Size::Small}}) {}

void SubtitleCell::drawRect(KDContext *ctx, KDRect rect) const {
  KDColor backColor = isHighlighted() ? Palette::Select : k_backgroundColor;
  drawInnerRect(ctx, bounds(), backColor);
  drawBorderOfRect(ctx, bounds(), Palette::GrayBright);
}

void SubtitleCell::layoutSubviews(bool force) {
  KDCoordinate width = bounds().width();
  KDCoordinate height = bounds().height();
  if (width == 0 || height == 0) {
    return;
  }
  KDCoordinate x = k_separatorThickness + Metric::CellLeftMargin;
  width -= x + Metric::CellRightMargin + k_separatorThickness;
  /* With an enforced 24px height, no vertical margins are enforced. Vertical
   * centering is handled in BufferTextView. Additionally, an offset is added at
   * the top to lower the baseline of the buffer view to look more centered. */
  KDCoordinate topOffset = 2;
  KDCoordinate y = k_separatorThickness + topOffset;
  height -= y + k_separatorThickness;

  assert(width > 0 && height > 0);

  setChildFrame(&m_textView, KDRect(x, y, width, height), force);
}

}  // namespace Code
