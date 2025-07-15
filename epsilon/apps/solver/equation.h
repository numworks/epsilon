#ifndef SOLVER_EQUATION_h
#define SOLVER_EQUATION_h

#include <apps/shared/expression_model_handle.h>

namespace Solver {

class Equation : public Shared::ExpressionModelHandle {
 public:
  Equation(Ion::Storage::Record record = Record())
      : ExpressionModelHandle(record) {}
  Poincare::SystemExpression standardForm(
      Poincare::Context* context, bool replaceFunctionsButNotSymbols,
      Poincare::ReductionTarget reductionTarget) const {
    return m_model.standardForm(this, context, replaceFunctionsButNotSymbols,
                                reductionTarget);
  }
  CodePoint symbol() const override { return 0; }

 private:
  class Model : public Shared::ExpressionModel {
   public:
    Poincare::SystemExpression standardForm(
        const Ion::Storage::Record* record, Poincare::Context* context,
        bool replaceFunctionsButNotSymbols,
        Poincare::ReductionTarget reductionTarget) const;
    /* The only difference with ExpressionModel is that we require units to have
     * an underscore, so that they are not mixed with symbols. */
    Poincare::UserExpression buildExpressionFromLayout(
        Poincare::Layout l, CodePoint symbol = 0,
        Poincare::Context* context = nullptr) const override;

   private:
    void* expressionAddress(const Ion::Storage::Record* record) const override;
    size_t expressionSize(const Ion::Storage::Record* record) const override;
    void setStorageChangeFlag() const override {}
  };
  size_t metaDataSize() const override { return 0; }
  const Shared::ExpressionModel* model() const override { return &m_model; }
  Model m_model;
};

}  // namespace Solver

#endif
