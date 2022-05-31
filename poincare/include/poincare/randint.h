#ifndef POINCARE_RANDINT_H
#define POINCARE_RANDINT_H

#include <poincare/expression.h>

namespace Poincare {

class RandintNode /*final*/ : public ExpressionNode  {
  friend class Randint;
public:

  // TreeNode
  size_t size() const override { return sizeof(RandintNode); }
  int numberOfChildren() const override;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "Randint";
  }
#endif

  // Properties
  Type type() const override { return Type::Randint; }

  bool isRandom() const override { return true; }

private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  // Simplification
  Expression shallowReduce(ReductionContext reductionContext) override;
  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, ApproximationContext approximationContext) const override {
    return templateApproximate<float>(approximationContext);
  }
  Evaluation<double> approximate(DoublePrecision p, ApproximationContext approximationContext) const override {
    return templateApproximate<double>(approximationContext);
  }
  template <typename T> Evaluation<T> templateApproximate(ApproximationContext approximationContext, bool * inputIsUndefined = nullptr) const;

  LayoutShape leftLayoutShape() const override { return LayoutShape::MoreLetters; };
  LayoutShape rightLayoutShape() const override { return LayoutShape::BoundaryPunctuation; }
};

class Randint final : public Expression {
friend class RandintNode;
public:
  Randint(const RandintNode * n) : Expression(n) {}
  static Randint Builder(Expression child0, Expression child1) { return TreeHandle::FixedArityBuilder<Randint, RandintNode>({child0, child1}); }
  static constexpr Expression::FunctionHelper s_functionHelper = Expression::FunctionHelper("randint", 2, &UntypedBuilderTwoChildren<Randint>);

  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
};


}

#endif
