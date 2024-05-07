#ifndef POINCARE_COMPLEX_CARTESIAN_H
#define POINCARE_COMPLEX_CARTESIAN_H

#include "old_expression.h"

namespace Poincare {

class Multiplication;

class ComplexCartesianNode : public ExpressionNode {
 public:
  // PoolObject
  size_t size() const override { return sizeof(ComplexCartesianNode); }
  int numberOfChildren() const override { return 2; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "ComplexCartesian";
  }
#endif

  // Properties
  OMG::Troolean isPositive(Context* context) const override {
    return childAtIndex(1)->isNull(context) == OMG::Troolean::True
               ? childAtIndex(0)->isPositive(context)
               : OMG::Troolean::Unknown;
  }
  OMG::Troolean isNull(Context* context) const override;
  Type otype() const override { return Type::ComplexCartesian; }

 private:
  // Layout
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
  OExpression shallowBeautify(
      const ReductionContext& reductionContext) override;
  OExpression shallowReduce(const ReductionContext& reductionContext) override;
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

class ComplexCartesian final : public OExpression {
 public:
  ComplexCartesian() : OExpression() {}
  ComplexCartesian(const ComplexCartesianNode* node) : OExpression(node) {}
  static ComplexCartesian Builder() {
    return PoolHandle::FixedArityBuilder<ComplexCartesian,
                                         ComplexCartesianNode>();
  }
  static ComplexCartesian Builder(OExpression child0, OExpression child1) {
    return PoolHandle::FixedArityBuilder<ComplexCartesian,
                                         ComplexCartesianNode>(
        {child0, child1});
  }

  // Getters
  OExpression real() const { return childAtIndex(0); }
  OExpression imag() const { return childAtIndex(1); }

  // Simplification
  OExpression shallowBeautify(const ReductionContext& reductionContext);
  OExpression shallowReduce(ReductionContext reductionContext);

  // Common operations (done in-place)
  OExpression squareNorm(const ReductionContext& reductionContext);
  OExpression norm(const ReductionContext& reductionContext);
  OExpression argument(const ReductionContext& reductionContext);
  ComplexCartesian inverse(const ReductionContext& reductionContext);
  ComplexCartesian squareRoot(const ReductionContext& reductionContext);
  ComplexCartesian powerInteger(int n,
                                const ReductionContext& reductionContext);
  ComplexCartesian add(ComplexCartesian& other,
                       const ReductionContext& reductionContext);
  ComplexCartesian multiply(ComplexCartesian& other,
                            const ReductionContext& reductionContext);
  ComplexCartesian power(ComplexCartesian& other,
                         const ReductionContext& reductionContext);

 private:
  constexpr static int k_maxNumberOfNodesBeforeInterrupting = 50;
  void factorAndArgumentOfFunction(OExpression e,
                                   ExpressionNode::Type searchedType,
                                   OExpression* factor, OExpression* argument,
                                   const ReductionContext& reductionContext);
  ComplexCartesian interruptComputationIfManyNodes();
  static Multiplication squareRootHelper(
      OExpression e, const ReductionContext& reductionContext);
  static OExpression powerHelper(OExpression norm, OExpression trigo,
                                 const ReductionContext& reductionContext);
};

}  // namespace Poincare

#endif
