#ifndef POINCARE_REGRESSION_MEDIAN_REGRESSION_H
#define POINCARE_REGRESSION_MEDIAN_REGRESSION_H

#include "affine_regression.h"

namespace Regression {

class MedianRegression : public AffineRegression {
 public:
  I18n::Message name() const override {
    return I18n::Message::MedianRegression;
  }

 private:
  double getMedianValue(Store* store, uint8_t* sortedIndex, int series,
                        int column, int startIndex, int endIndex);
  void privateFit(Store* store, int series, double* modelCoefficients,
                  Poincare::Context* context) override;
};

}  // namespace Regression

#endif
