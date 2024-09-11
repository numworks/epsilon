#ifndef ESCHER_METRIC_H
#define ESCHER_METRIC_H

#include <ion/display.h>
#include <kandinsky/coordinate.h>
#include <kandinsky/font.h>
#include <kandinsky/margins.h>

namespace Escher {

class Metric {
 public:
  // Common margins
  constexpr static KDMargins CommonMargins = KDMargins(14);
  constexpr static KDCoordinate CommonLargeMargin = 10;
  constexpr static KDCoordinate CommonSmallMargin = 5;
  constexpr static KDCoordinate ScrollViewBarThickness = 2;
  constexpr static KDCoordinate CommonMenuMargin =
      (CommonMargins.left() - ScrollViewBarThickness) / 2;

  // General cell margins
  constexpr static KDMargins CellMargins = KDMargins(8);
  constexpr static KDCoordinate CellHorizontalElementMargin =
      CellMargins.left();
  constexpr static KDCoordinate CellSeparatorThickness = 1;
  constexpr static KDCoordinate CellVerticalElementMargin = 4;

  // Pop-up margins
  constexpr static KDMargins PopUpMargins = {27, 27, 27, 55};
  constexpr static KDMargins PopUpMarginsNoBottom = {27, 27, 27, 0};

  // Misc margins
  constexpr static KDCoordinate BannerTextMargin = 2;
  constexpr static KDCoordinate EllipsisCellWidth = 37;
  constexpr static KDCoordinate LayoutViewMargin = 5;
  constexpr static KDCoordinate ExpressionInputBarMinimalHeight = 37;
  constexpr static KDCoordinate EditableExpressionAdditionalMargin =
      CellMargins.left() - LayoutViewMargin;
  constexpr static KDCoordinate SmallCellMargin = 2;
  constexpr static KDCoordinate TableSeparatorThickness = 5;
  constexpr static KDCoordinate TitleBarExternHorizontalMargin = 5;

  // ButtonRow
  constexpr static KDCoordinate ButtonRowPlainStyleHeight = 20;
  constexpr static KDCoordinate ButtonRowEmbossedStyleHeightSmall = 36;
  constexpr static KDCoordinate ButtonRowEmbossedStyleHeightLarge = 52;

  // Color indicators
  constexpr static KDCoordinate HorizontalColorIndicatorThickness = 2;
  constexpr static KDCoordinate VerticalColorIndicatorThickness = 3;

  // Misc
  constexpr static KDCoordinate TitleBarHeight = 18;
  constexpr static KDCoordinate StackTitleHeight = 20;
  constexpr static KDCoordinate TabHeight = 27;
  constexpr static KDCoordinate ScrollStep = 10;
  constexpr static KDCoordinate StoreRowHeight = 50;
  constexpr static KDCoordinate SmallEditableCellHeight = 20;

  // Display
  constexpr static KDCoordinate DisplayHeightWithoutTitleBar =
      Ion::Display::Height - TitleBarHeight;

  constexpr static KDCoordinate SmallFontCellWidth(int numberOfChars,
                                                   KDCoordinate margin) {
    return numberOfChars * KDFont::GlyphWidth(KDFont::Size::Small) + 2 * margin;
  }

  constexpr static int MinimalNumberOfScrollableRowsToFillDisplayHeight(
      KDCoordinate rowHeight, KDCoordinate unusableHeight = 0) {
    return 1 + 1 +
           ((DisplayHeightWithoutTitleBar - unusableHeight - 1) / rowHeight);
    /* The -1 and one of the +1 ensures that we ceil the result.
     * The other +1 is to take into account that the rows are scrollable
     * so there can be half of one at the top and half of another one at the
     * bottom.
     * */
  }
};

}  // namespace Escher
#endif
