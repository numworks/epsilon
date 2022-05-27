#ifndef REGRESSION_MEDIAN_MODEL_H
#define REGRESSION_MEDIAN_MODEL_H

#include "affine_model.h"

namespace Regression {

class MedianModel : public AffineModel {
public:
  I18n::Message name() const override { return I18n::Message::MedianRegression; }

private:
  double getMedianValue(Store * store, uint8_t * sortedIndex, int series, int column, int startIndex, int endIndex);
  void privateFit(Store * store, int series, double * modelCoefficients, Poincare::Context * context) override;
};

}


#endif
