#ifndef SOLVER_EQUATION_STORE_H
#define SOLVER_EQUATION_STORE_H

#include "equation.h"
#include "../shared/expression_model_store.h"
#include <stdint.h>

namespace Solver {

class EquationStore : public Shared::ExpressionModelStore {
public:
  EquationStore() {}
  Equation * modelAtIndex(int i) override {
    assert(i>=0 && i<m_numberOfModels);
    return &m_equations[i];
  }
  int maxNumberOfModels() const override { return k_maxNumberOfEquations; }
private:
  Equation * emptyModel() override;
  Equation * nullModel() override {
    return emptyModel();
  }
  void setModelAtIndex(Shared::ExpressionModel * f, int i) override;
  static constexpr int k_maxNumberOfEquations = 6;
  Equation m_equations[k_maxNumberOfEquations];;
};

}

#endif
