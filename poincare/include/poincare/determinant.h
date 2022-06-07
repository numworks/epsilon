#ifndef POINCARE_DETERMINANT_H
#define POINCARE_DETERMINANT_H

#include <poincare/expression.h>

namespace Poincare {

class DeterminantNode /*final*/ : public ExpressionNode {
public:
  static constexpr char functionName[] = "det";

  // TreeNode
  size_t size() const override { return sizeof(DeterminantNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "Determinant";
  }
#endif

  Type type() const override { return Type::Determinant; }
private:
  /* Layout */
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  /* Serialization */
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  /* Simplification */
  Expression shallowReduce(ReductionContext reductionContext) override;
  LayoutShape leftLayoutShape() const override { return LayoutShape::MoreLetters; };
  LayoutShape rightLayoutShape() const override { return LayoutShape::BoundaryPunctuation; }
  /* Approximation */
  Evaluation<float> approximate(SinglePrecision p, ApproximationContext approximationContext) const override { return templatedApproximate<float>(approximationContext); }
  Evaluation<double> approximate(DoublePrecision p, ApproximationContext approximationContext) const override { return templatedApproximate<double>(approximationContext); }
 template<typename T> Evaluation<T> templatedApproximate(ApproximationContext approximationContext) const;
};


class Determinant final : public HandleOneChild<Determinant, DeterminantNode> {
public:
  using Handle::Handle, Handle::Builder, Handle::s_functionHelper;
  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
};

}

#endif
