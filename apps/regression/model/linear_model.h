#ifndef REGRESSION_LINEAR_MODEL_H
#define REGRESSION_LINEAR_MODEL_H

#include "affine_model.h"

namespace Regression {

class LinearModel : public AffineModel {
public:
  void fit(Store * store, int series, double * modelCoefficients, Poincare::Context * context) override;
};

}


#endif
