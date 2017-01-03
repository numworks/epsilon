#ifndef ESCHER_METRIC_H
#define ESCHER_METRIC_H

#include <kandinsky/coordinate.h>

class Metric {
public:
  constexpr static KDCoordinate LeftMargin = 20;
  constexpr static KDCoordinate RightMargin = 20;
  constexpr static KDCoordinate TopMargin = 10;
  constexpr static KDCoordinate BottomMargin = 10;
  constexpr static KDCoordinate LabelMargin = 5;
  constexpr static KDCoordinate ParameterCellHeight = 35;
};

#endif