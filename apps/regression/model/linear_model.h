#ifndef REGRESSION_LINEAR_MODEL_H
#define REGRESSION_LINEAR_MODEL_H

#include "affine_model.h"

namespace Regression {

class LinearModel : public AffineModel {
public:
  LinearModel(bool isApbxForm = false) : m_isApbxForm(isApbxForm) {}
  I18n::Message formulaMessage() const override { return m_isApbxForm ? I18n::Message::LinearApbxRegressionFormula : AffineModel::formulaMessage(); }
  I18n::Message name() const override { return I18n::Message::Linear; }

private:
  void privateFit(Store * store, int series, double * modelCoefficients, Poincare::Context * context) override;
  /* In a+bx form, Coefficients are swapped */
  int slopeCoefficientIndex() const override { return m_isApbxForm; }
  int yInterceptCoefficientIndex() const override { return !m_isApbxForm; }
  const char * layoutString() const override { return m_isApbxForm ? "a+b·x" : AffineModel::layoutString(); }
  const char * equationTemplate() const override { return m_isApbxForm ? "%*.*ed%+*.*ed·x" : AffineModel::equationTemplate(); }

  bool m_isApbxForm;
};

}


#endif
