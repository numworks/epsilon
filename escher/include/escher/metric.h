#ifndef ESCHER_METRIC_H
#define ESCHER_METRIC_H

#include <kandinsky/coordinate.h>
#include <kandinsky/font.h>
#include <ion/display.h>

namespace Escher {

class Metric {
public:
  // Common margins
  constexpr static KDCoordinate CommonMargin = 14;
  constexpr static KDCoordinate ScrollViewBarThickness = 2;
  constexpr static KDCoordinate CommonBottomMargin = CommonMargin;
  constexpr static KDCoordinate CommonLargeMargin = 10;
  constexpr static KDCoordinate CommonLeftMargin = CommonMargin;
  constexpr static KDCoordinate CommonMenuMargin = (CommonMargin - ScrollViewBarThickness)/2;
  constexpr static KDCoordinate CommonRightMargin = CommonMargin;
  constexpr static KDCoordinate CommonSmallMargin = 5;
  constexpr static KDCoordinate CommonTopMargin = CommonMargin;

  // General cell margins
  constexpr static KDCoordinate BigCellMargin = 8;
  constexpr static KDCoordinate CellBottomMargin = BigCellMargin;
  constexpr static KDCoordinate CellHorizontalElementMargin = BigCellMargin;
  constexpr static KDCoordinate CellLeftMargin = BigCellMargin;
  constexpr static KDCoordinate CellRightMargin = BigCellMargin;
  constexpr static KDCoordinate CellTopMargin = BigCellMargin;
  constexpr static KDCoordinate CellSeparatorThickness = 1;
  constexpr static KDCoordinate CellVerticalElementMargin = 4;

  // Pop-up margins
  constexpr static KDCoordinate PopUpMargin = 27;
  constexpr static KDCoordinate PopUpBottomMargin = 55;
  constexpr static KDCoordinate PopUpLeftMargin = PopUpMargin;
  constexpr static KDCoordinate PopUpRightMargin = PopUpMargin;
  constexpr static KDCoordinate PopUpTopMargin = PopUpMargin;

  // Misc margins
  constexpr static KDCoordinate BannerTextMargin = 2;
  constexpr static KDCoordinate EllipsisCellWidth = 37;
  constexpr static KDCoordinate ExpressionViewHorizontalMargin = 5;
  constexpr static KDCoordinate FractionAndConjugateHorizontalMargin = 2;
  constexpr static KDCoordinate FractionAndConjugateHorizontalOverflow = 2;
  constexpr static KDCoordinate MinimalBracketAndParenthesisChildHeight = 16;
  constexpr static KDCoordinate OperatorHorizontalMargin = 4;
  constexpr static KDCoordinate SmallCellMargin = 2;
  constexpr static KDCoordinate TableSeparatorThickness = 5;
  constexpr static KDCoordinate ThousandsSeparatorWidth = 3;
  constexpr static KDCoordinate TitleBarExternHorizontalMargin = 5;

  // ButtonRow
  constexpr static KDCoordinate ButtonRowPlainStyleHeight = 20;
  constexpr static KDCoordinate ButtonRowEmbossedStyleHeightSmall = 36;
  constexpr static KDCoordinate ButtonRowEmbossedStyleHeightLarge = 52;
  constexpr static KDCoordinate ButtonRowEmbossedStyleHeightMarginSmall = 6;
  constexpr static KDCoordinate ButtonRowEmbossedStyleHeightMarginLarge = 8;

  // Misc
  constexpr static KDCoordinate TitleBarHeight = 18;
  constexpr static KDCoordinate StackTitleHeight = 20;
  constexpr static KDCoordinate TabHeight = 27;
  constexpr static KDCoordinate ScrollStep = 10;
  constexpr static KDCoordinate StoreRowHeight = 50;
  constexpr static KDCoordinate SmallEditableCellHeight = 20;

  // Display
  constexpr static KDCoordinate DisplayHeightWithoutTitleBar = Ion::Display::Height - TitleBarHeight;

  constexpr static KDCoordinate SmallFontCellWidth(int numberOfChars, KDCoordinate margin) {
    return numberOfChars * KDFont::GlyphWidth(KDFont::Size::Small) + 2 * margin;
  }

  constexpr static int MinimalNumberOfScrollableRowsToFillDisplayHeight(KDCoordinate rowHeight, KDCoordinate unusableHeight = 0) {
    return 1 + 1 + ((DisplayHeightWithoutTitleBar - unusableHeight - 1) / rowHeight);
    /* The -1 and one of the +1 ensures that we ceil the result.
     * The other +1 is to take into account that the rows are scrollable
     * so there can be half of one at the top and half of another one at the
     * bottom.
     * */
  }
};

}
#endif
