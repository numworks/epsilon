#pragma once

#include "affine_regression.h"

namespace Poincare::Internal {

class MedianRegression : public AffineRegression {
 public:
  Type type() const override { return Type::Median; }

 private:
  double getMedianValue(const Series* series, uint8_t* sortedIndex, int column,
                        int startIndex, int endIndex) const;
  Coefficients privateFit(const Series* series) const override;
};

}  // namespace Poincare::Internal
