#ifndef ESCHER_METRIC_H
#define ESCHER_METRIC_H

#include <kandinsky/coordinate.h>

class Metric {
public:
  constexpr static KDCoordinate CommonLeftMargin = 20;
  constexpr static KDCoordinate CommonRightMargin = 20;
  constexpr static KDCoordinate CommonTopMargin = 15;
  constexpr static KDCoordinate CommonBottomMargin = 15;
  constexpr static KDCoordinate ParameterCellHeight = 35;
  constexpr static KDCoordinate ModalTopMargin = 5;
  constexpr static KDCoordinate ModalBottomMargin = 20;
  constexpr static KDCoordinate TabHeight = 27;
  constexpr static KDCoordinate ScrollStep = 10;
};

#endif