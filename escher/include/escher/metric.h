#ifndef ESCHER_METRIC_H
#define ESCHER_METRIC_H

#include <kandinsky/coordinate.h>

namespace Escher {

class Metric {
public:
  constexpr static KDCoordinate CellTopMargin = 8;
  constexpr static KDCoordinate CellRightMargin = 8;
  constexpr static KDCoordinate CellLeftMargin = 8;
  constexpr static KDCoordinate CellBottomMargin = 8;
  constexpr static KDCoordinate CellHorizontalElementMargin = 8;
  constexpr static KDCoordinate CellVerticalElementMargin = 4;
  constexpr static KDCoordinate CellMargin = 2;
  constexpr static KDCoordinate CommonLeftMargin = 20;
  constexpr static KDCoordinate CommonRightMargin = 20;
  constexpr static KDCoordinate CommonTopMargin = 15;
  constexpr static KDCoordinate CommonBottomMargin = 15;
  constexpr static KDCoordinate CommonLargeMargin = 10;
  constexpr static KDCoordinate CommonSmallMargin = 5;
  constexpr static KDCoordinate TitleBarExternHorizontalMargin = 5;
  constexpr static KDCoordinate TitleBarHeight = 18;
  constexpr static KDCoordinate TabHeight = 27;
  constexpr static KDCoordinate ScrollStep = 10;
  constexpr static KDCoordinate PopUpLeftMargin = 27;
  constexpr static KDCoordinate PopUpRightMargin = 27;
  constexpr static KDCoordinate PopUpTopMargin = 27;
  constexpr static KDCoordinate PopUpBottomMargin = 55;
  constexpr static KDCoordinate StoreRowHeight = 50;
  constexpr static KDCoordinate StackTitleHeight = 20;
  constexpr static KDCoordinate FractionAndConjugateHorizontalOverflow = 2;
  constexpr static KDCoordinate FractionAndConjugateHorizontalMargin = 2;
  constexpr static KDCoordinate MinimalBracketAndParenthesisHeight = 18;
  constexpr static KDCoordinate CellSeparatorThickness = 1;
  constexpr static KDCoordinate TableSeparatorThickness = 5;
  constexpr static KDCoordinate ExpressionViewHorizontalMargin = 5;
  constexpr static KDCoordinate EllipsisCellWidth = 37;
};

}
#endif
