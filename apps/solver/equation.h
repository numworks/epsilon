#ifndef SOLVER_EQUATION_h
#define SOLVER_EQUATION_h

#include "../shared/single_expression_model_handle.h"

namespace Solver {

class Equation : public Shared::SingleExpressionModelHandle {
public:
  Equation(Ion::Storage::Record record = Record());
  bool shouldBeClearedBeforeRemove() override {
    return false;
  }
  Poincare::Expression standardForm(Poincare::Context * context) const { return m_handle.standardForm(this, context); }
  bool containsIComplex(Poincare::Context * context) const;

private:
  class Handle : public Shared::ExpressionModelHandle {
  public:
    Poincare::Expression standardForm(const Ion::Storage::Record * record, Poincare::Context * context) const;
    void tidy() const override;
    void * expressionAddress(const Ion::Storage::Record * record) const override;
  private:
    size_t expressionSize(const Ion::Storage::Record * record) const override;
    mutable Poincare::Expression m_standardForm;
  };
  size_t metaDataSize() const override { return 0; }
  const Shared::ExpressionModelHandle * handle() const override { return &m_handle; }
  Handle m_handle;
};

}

#endif
