#pragma once

#include <apps/shared/expression_model_store.h>
#include <poincare/equation_solver/equation_solver_pool.h>

#include "equation.h"

namespace Solver {

class EquationStore : public Shared::ExpressionModelStore {
 public:
  constexpr static int k_maxNumberOfEquations =
      Poincare::EquationSolver::k_maxNumberOfVariables;
  constexpr static const char* k_extension = Ion::Storage::equationExtension;

  // ExpressionModelStore
  Ion::Storage::Record::ErrorStatus addEmptyModel() override;

  Poincare::UserExpression cloneExpressionOfEquationAtIndex(int i) const {
    return modelForRecord(definedRecordAtIndex(i))->expressionClone();
  }

  OMG::ExpiringPointer<Equation> modelForRecord(Ion::Storage::Record record) {
    return OMG::ExpiringPointer<Equation>(
        static_cast<Equation*>(privateModelForRecord(record)));
  }
  OMG::ExpiringPointer<const Equation> modelForRecord(
      Ion::Storage::Record record) const {
    return OMG::ExpiringPointer<const Equation>(
        static_cast<const Equation*>(privateModelForRecord(record)));
  }
  static void RemoveAll() {
    Shared::ExpressionModelStore::RemoveAll(k_extension);
  }

 private:
  // ExpressionModelStore
  const char* modelExtension() const override { return k_extension; }
  int maxNumberOfMemoizedModels() const override {
    return k_maxNumberOfEquations;
  }
  Shared::ExpressionModelHandle* setMemoizedModelAtIndex(
      int cacheIndex, Ion::Storage::Record) const override;
  Shared::ExpressionModelHandle* memoizedModelAtIndex(
      int cacheIndex) const override;

  mutable Equation m_equations[k_maxNumberOfEquations];
};

}  // namespace Solver
