#ifndef ESCHER_METRIC_H
#define ESCHER_METRIC_H

#include <kandinsky/coordinate.h>

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
  constexpr static KDCoordinate MinimalBracketAndParenthesisHeight = 18;
  constexpr static KDCoordinate OperatorHorizontalMargin = 4;
  constexpr static KDCoordinate ScrollStep = 10;
  constexpr static KDCoordinate SmallCellMargin = 2;
  constexpr static KDCoordinate StackTitleHeight = 20;
  constexpr static KDCoordinate StoreRowHeight = 50;
  constexpr static KDCoordinate TabHeight = 27;
  constexpr static KDCoordinate TableSeparatorThickness = 5;
  constexpr static KDCoordinate ThousandsSeparatorWidth = 3;
  constexpr static KDCoordinate TitleBarExternHorizontalMargin = 5;
  constexpr static KDCoordinate TitleBarHeight = 18;

  constexpr static KDCoordinate SmallFontCellWidth(int numberOfChars, KDCoordinate margin) {
    /* KDFont::SmallFont->glyphSize().width() = 7
     * TODO: Replace 7 by a constexpr */
    return numberOfChars * 7 + 2 * margin;
  }
};

}
#endif
