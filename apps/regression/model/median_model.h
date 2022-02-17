#ifndef REGRESSION_MEDIAN_MODEL_H
#define REGRESSION_MEDIAN_MODEL_H

#include "affine_model.h"

namespace Regression {

class MedianModel : public AffineModel {
public:
  void fit(Store * store, int series, double * modelCoefficients, Poincare::Context * context) override;

private:
  double getMedianValue(Store * store, int * sortedIndex, int series, int column, int startIndex, int endIndex);
};

}


#endif
