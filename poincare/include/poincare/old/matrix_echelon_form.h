#ifndef POINCARE_MATRIX_ECHELON_FORM_H
#define POINCARE_MATRIX_ECHELON_FORM_H

#include "old_expression.h"

namespace Poincare {

class MatrixEchelonFormNode : public ExpressionNode {
 public:
  // PoolObject
  int numberOfChildren() const override;
  virtual bool isFormReduced() const = 0;
  constexpr static int sNumberOfChildren = 1;

 private:
  // Layout
  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfSignificantDigits) const override;
  // Simplification
  OExpression shallowReduce(const ReductionContext& reductionContext) override;
  LayoutShape leftLayoutShape() const override {
    return LayoutShape::MoreLetters;
  };
  LayoutShape rightLayoutShape() const override {
    return LayoutShape::BoundaryPunctuation;
  }
  // Evaluation
  Evaluation<float> approximate(
      SinglePrecision p,
      const ApproximationContext& approximationContext) const override {
    return templatedApproximate<float>(approximationContext);
  }
  Evaluation<double> approximate(
      DoublePrecision p,
      const ApproximationContext& approximationContext) const override {
    return templatedApproximate<double>(approximationContext);
  }
  template <typename T>
  Evaluation<T> templatedApproximate(
      const ApproximationContext& approximationContext) const;

  // Properties
  virtual const char* functionHelperName() const = 0;
};

class MatrixEchelonForm : public OExpression {
 public:
  MatrixEchelonForm(const MatrixEchelonFormNode* n) : OExpression(n) {}
  OExpression shallowReduce(ReductionContext reductionContext);
  bool isFormReduced() const {
    return static_cast<MatrixEchelonFormNode*>(node())->isFormReduced();
  }
};

}  // namespace Poincare

#endif
