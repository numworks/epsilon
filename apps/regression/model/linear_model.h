#ifndef REGRESSION_LINEAR_MODEL_H
#define REGRESSION_LINEAR_MODEL_H

#include "affine_model.h"

namespace Regression {

class LinearModel : public AffineModel {
public:
  I18n::Message name() const override { return I18n::Message::Linear; }
private:
  void privateFit(Store * store, int series, double * modelCoefficients, Poincare::Context * context) override;
};

}


#endif
