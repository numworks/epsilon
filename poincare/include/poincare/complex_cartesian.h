#ifndef POINCARE_COMPLEX_CARTESIAN_H
#define POINCARE_COMPLEX_CARTESIAN_H

#include <poincare/expression.h>
#include <poincare/multiplication.h>

namespace Poincare {

class ComplexCartesianNode : public ExpressionNode {
 public:
  // TreeNode
  size_t size() const override { return sizeof(ComplexCartesianNode); }
  int numberOfChildren() const override { return 2; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "ComplexCartesian";
  }
#endif

  // Properties
  TrinaryBoolean isPositive(Context* context) const override {
    return childAtIndex(1)->isNull(context) == TrinaryBoolean::True
               ? childAtIndex(0)->isPositive(context)
               : TrinaryBoolean::Unknown;
  }
  TrinaryBoolean isNull(Context* context) const override;
  Type type() const override { return Type::ComplexCartesian; }

 private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode,
                      int numberOfSignificantDigits,
                      Context* context) const override {
    assert(false);
    return Layout();
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
  // Simplification
  Expression shallowBeautify(const ReductionContext& reductionContext) override;
  Expression shallowReduce(const ReductionContext& reductionContext) override;
  LayoutShape leftLayoutShape() const override {
    /* leftLayoutShape is called after beautifying expression. ComplexCartesian
     * is transformed in another expression at beautifying. */
    assert(false);
    return LayoutShape::BoundaryPunctuation;
  };

  template <typename T>
  Complex<T> templatedApproximate(
      const ApproximationContext& approximationContext) const;
};

class ComplexCartesian final : public Expression {
 public:
  ComplexCartesian() : Expression() {}
  ComplexCartesian(const ComplexCartesianNode* node) : Expression(node) {}
  static ComplexCartesian Builder() {
    return TreeHandle::FixedArityBuilder<ComplexCartesian,
                                         ComplexCartesianNode>();
  }
  static ComplexCartesian Builder(Expression child0, Expression child1) {
    return TreeHandle::FixedArityBuilder<ComplexCartesian,
                                         ComplexCartesianNode>(
        {child0, child1});
  }

  // Getters
  Expression real() const { return childAtIndex(0); }
  Expression imag() const { return childAtIndex(1); }

  // Simplification
  Expression shallowBeautify(const ReductionContext& reductionContext);
  Expression shallowReduce(ReductionContext reductionContext);

  // Common operations (done in-place)
  Expression squareNorm(const ReductionContext& reductionContext);
  Expression norm(const ReductionContext& reductionContext);
  Expression argument(const ReductionContext& reductionContext);
  ComplexCartesian inverse(const ReductionContext& reductionContext);
  ComplexCartesian squareRoot(const ReductionContext& reductionContext);
  ComplexCartesian powerInteger(int n,
                                const ReductionContext& reductionContext);
  ComplexCartesian multiply(ComplexCartesian& other,
                            const ReductionContext& reductionContext);
  ComplexCartesian power(ComplexCartesian& other,
                         const ReductionContext& reductionContext);

 private:
  constexpr static int k_maxNumberOfNodesBeforeInterrupting = 50;
  void factorAndArgumentOfFunction(Expression e,
                                   ExpressionNode::Type searchedType,
                                   Expression* factor, Expression* argument,
                                   const ReductionContext& reductionContext);
  ComplexCartesian interruptComputationIfManyNodes();
  static Multiplication squareRootHelper(
      Expression e, const ReductionContext& reductionContext);
  static Expression powerHelper(Expression norm, Expression trigo,
                                const ReductionContext& reductionContext);
};

}  // namespace Poincare

#endif
