#ifndef POINCARE_SUBSTRACTION_H
#define POINCARE_SUBSTRACTION_H

#include <poincare/approximation_helper.h>
#include <poincare/old_expression.h>

namespace Poincare {

class SubtractionNode final : public ExpressionNode {
 public:
  // PoolObject
  size_t size() const override { return sizeof(SubtractionNode); }
  int numberOfChildren() const override { return 2; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream& stream) const override {
    stream << "Subtraction";
  }
#endif

  // ExpressionNode

  // Properties
  Type otype() const override { return Type::Subtraction; }
  bool childAtIndexNeedsUserParentheses(const OExpression& child,
                                        int childIndex) const override;
  OExpression removeUnit(OExpression* unit) override {
    assert(false);
    return ExpressionNode::removeUnit(unit);
  }

  // Approximation
  template <typename T>
  static std::complex<T> computeOnComplex(
      const std::complex<T> c, const std::complex<T> d,
      Preferences::ComplexFormat complexFormat) {
    return c - d;
  }

  template <typename T>
  static Evaluation<T> Compute(Evaluation<T> eval1, Evaluation<T> eval2,
                               Preferences::ComplexFormat complexFormat) {
    return ApproximationHelper::Reduce<T>(
        eval1, eval2, complexFormat, computeOnComplex<T>,
        ApproximationHelper::UndefinedOnComplexAndMatrix<T>,
        ApproximationHelper::UndefinedOnMatrixAndComplex<T>,
        computeOnMatrices<T>);
  }

  Evaluation<float> approximate(
      SinglePrecision p,
      const ApproximationContext& approximationContext) const override {
    return ApproximationHelper::MapReduce<float>(this, approximationContext,
                                                 Compute<float>);
  }
  Evaluation<double> approximate(
      DoublePrecision p,
      const ApproximationContext& approximationContext) const override {
    return ApproximationHelper::MapReduce<double>(this, approximationContext,
                                                  Compute<double>);
  }

  /* Layout */
  size_t serialize(char* buffer, size_t bufferSize,
                   Preferences::PrintFloatMode floatDisplayMode,
                   int numberOfSignificantDigits) const override;

  /* Simplification */
  OExpression shallowReduce(const ReductionContext& reductionContext) override;

 private:
  /* Simplification */
  LayoutShape leftLayoutShape() const override {
    return childAtIndex(0)->leftLayoutShape();
  };
  LayoutShape rightLayoutShape() const override {
    return childAtIndex(1)->rightLayoutShape();
  }
  /* Evaluation */
  template <typename T>
  static MatrixComplex<T> computeOnMatrices(
      const MatrixComplex<T> m, const MatrixComplex<T> n,
      Preferences::ComplexFormat complexFormat) {
    return ApproximationHelper::ElementWiseOnMatrices(m, n, complexFormat,
                                                      computeOnComplex<T>);
  }
};

class Subtraction final
    : public ExpressionTwoChildren<Subtraction, SubtractionNode> {
 public:
  using ExpressionBuilder::ExpressionBuilder, ExpressionBuilder::Builder;
  static Subtraction Builder() {
    return PoolHandle::FixedArityBuilder<Subtraction, SubtractionNode>();
  }
  // OExpression
  OExpression shallowReduce(ReductionContext reductionContext);
};

}  // namespace Poincare

#endif
