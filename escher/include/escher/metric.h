#ifndef ESCHER_METRIC_H
#define ESCHER_METRIC_H

#include <kandinsky/coordinate.h>

class Metric {
public:
  constexpr static KDCoordinate CommonLeftMargin = 20;
  constexpr static KDCoordinate CommonRightMargin = 20;
  constexpr static KDCoordinate CommonTopMargin = 15;
  constexpr static KDCoordinate CommonBottomMargin = 15;
  constexpr static KDCoordinate HistoryHorizontalMargin = 10;
  constexpr static KDCoordinate TitleBarExternHorizontalMargin = 5;
  constexpr static KDCoordinate TitleBarHeight = 18;
  constexpr static KDCoordinate ParameterCellHeight = 35;
  constexpr static KDCoordinate ModalTopMargin = 5;
  constexpr static KDCoordinate ModalBottomMargin = 18;
  constexpr static KDCoordinate TableCellLabelTopMargin = 3;
  constexpr static KDCoordinate TabHeight = 27;
  constexpr static KDCoordinate ScrollStep = 10;
  constexpr static KDCoordinate PopUpLeftMargin = 32;
  constexpr static KDCoordinate PopUpRightMargin = 32;
  constexpr static KDCoordinate PopUpTopMargin = 50;
  constexpr static KDCoordinate ExamPopUpTopMargin = 27;
  constexpr static KDCoordinate ExamPopUpBottomMargin = 55;
  constexpr static KDCoordinate StoreRowHeight = 50;
  constexpr static KDCoordinate ToolboxRowHeight = 40;
  constexpr static KDCoordinate StackTitleHeight = 20;
};

#endif
