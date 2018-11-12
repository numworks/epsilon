#ifndef SOLVER_EQUATION_h
#define SOLVER_EQUATION_h

#include "../shared/expression_model.h"

namespace Solver {

class Equation : public Shared::ExpressionModel {
public:
  Equation();
  void setContent(const char * c) override;
  void tidy() override;
  bool shouldBeClearedBeforeRemove() override {
    return false;
  }
  Poincare::Expression standardForm(Poincare::Context * context) const;
private:
  void tidyStandardForm();
  mutable Poincare::Expression m_standardForm;
};

}

#endif
