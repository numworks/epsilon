#ifndef POINCARE_MATRIX_ECHELON_FORM_H
#define POINCARE_MATRIX_ECHELON_FORM_H

#include <poincare/expression.h>

namespace Poincare {

class MatrixEchelonFormNode : public ExpressionNode {
public:

  // TreeNode
  int numberOfChildren() const override;
  virtual bool isFormReduced() const = 0;
  static constexpr int sNumberOfChildren = 1;
private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  // Simplification
  Expression shallowReduce(const ReductionContext& reductionContext) override;
  LayoutShape leftLayoutShape() const override { return LayoutShape::MoreLetters; };
  LayoutShape rightLayoutShape() const override { return LayoutShape::BoundaryPunctuation; }
  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, const ApproximationContext& approximationContext) const override { return templatedApproximate<float>(approximationContext); }
  Evaluation<double> approximate(DoublePrecision p, const ApproximationContext& approximationContext) const override { return templatedApproximate<double>(approximationContext); }
  template<typename T> Evaluation<T> templatedApproximate(const ApproximationContext& approximationContext) const;

  // Properties
  virtual const char * functionHelperName() const = 0;
};

class MatrixEchelonForm : public Expression {
public:
  MatrixEchelonForm(const MatrixEchelonFormNode * n) : Expression(n) {}
  Expression shallowReduce(const ExpressionNode::ReductionContext& reductionContext);
  bool isFormReduced() const { return static_cast<MatrixEchelonFormNode *>(node())->isFormReduced(); }
};

}

#endif
