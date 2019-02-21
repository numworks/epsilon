#ifndef SOLVER_EQUATION_h
#define SOLVER_EQUATION_h

#include "../shared/expression_model_handle.h"

namespace Solver {

class Equation : public Shared::ExpressionModelHandle {
public:
  Equation(Ion::Storage::Record record = Record());
  void tidyExpressionModel() override;
  bool shouldBeClearedBeforeRemove() override {
    return false;
  }
  Poincare::Expression standardForm(Poincare::Context * context) const;
  bool containsIComplex(Poincare::Context * context) const;

  static constexpr char extension[] = "eq"; // TODO: store this elsewhere?
private:
  size_t metaDataSize() const override { return 0; }
  mutable Poincare::Expression m_standardForm;
};

}

#endif
