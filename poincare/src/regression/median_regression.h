#ifndef POINCARE_REGRESSION_MEDIAN_REGRESSION_H
#define POINCARE_REGRESSION_MEDIAN_REGRESSION_H

#include "affine_regression.h"

namespace Poincare::Regression {

class MedianRegression : public AffineRegression {
 public:
  Type type() const override { return Type::Median; }

 private:
  double getMedianValue(const Series* series, uint8_t* sortedIndex, int column,
                        int startIndex, int endIndex) const;
  CoefficientsType privateFit(const Series* series,
                              Poincare::Context* context) const override;
};

}  // namespace Poincare::Regression

#endif
