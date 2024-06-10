#ifndef POINCARE_REGRESSION_MEDIAN_REGRESSION_H
#define POINCARE_REGRESSION_MEDIAN_REGRESSION_H

#include "affine_regression.h"

namespace Poincare::Regression {

class MedianRegression : public AffineRegression {
 private:
  double getMedianValue(const Series* series, uint8_t* sortedIndex, int column,
                        int startIndex, int endIndex);
  void privateFit(const Series* series, double* modelCoefficients,
                  Poincare::Context* context) override;
};

}  // namespace Poincare::Regression

#endif
