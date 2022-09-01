#ifndef POINCARE_DIVISION_H
#define POINCARE_DIVISION_H

#include <poincare/expression.h>
#include <poincare/approximation_helper.h>

namespace Poincare {

class Division;

class DivisionNode /*final*/ : public ExpressionNode {
  friend class LogarithmNode;
public:

  // TreeNode
  size_t size() const override { return sizeof(DivisionNode); }
  int numberOfChildren() const override { return 2; }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override {
    stream << "Division";
  }
#endif

  // Properties
  TrinaryBoolean isPositive(Context * context) const override;
  TrinaryBoolean isNull(Context * context) const override {
    // NonNull Status can't be returned because denominator could be infinite.
    return TrinaryOr(childAtIndex(0)->isNull(context), TrinaryBoolean::Unknown);
  }
  Type type() const override { return Type::Division; }
  int polynomialDegree(Context * context, const char * symbolName) const override;
  Expression removeUnit(Expression * unit) override { assert(false); return ExpressionNode::removeUnit(unit); }

  // Approximation
  template<typename T> static Evaluation<T> Compute(Evaluation<T> eval1, Evaluation<T> eval2, Preferences::ComplexFormat complexFormat) {
    return ApproximationHelper::Reduce<T>(
        eval1,
        eval2,
        complexFormat,
        computeOnComplex<T>,
        ApproximationHelper::UndefinedOnComplexAndMatrix<T>,
        computeOnMatrixAndComplex<T>,
        ApproximationHelper::UndefinedOnMatrixAndMatrix<T>
        );
  }
  Evaluation<float> approximate(SinglePrecision p, const ApproximationContext& approximationContext) const override {
    return ApproximationHelper::MapReduce<float>(this, approximationContext, Compute<float>);
   }
  Evaluation<double> approximate(DoublePrecision p, const ApproximationContext& approximationContext) const override {
    return ApproximationHelper::MapReduce<double>(this, approximationContext, Compute<double>);
  }

  // Layout
  bool childNeedsSystemParenthesesAtSerialization(const TreeNode * child) const override;
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits, Context * context) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  // Simplification
  Expression shallowReduce(const ReductionContext& reductionContext) override;
  LayoutShape leftLayoutShape() const override { return LayoutShape::Fraction; };

private:
  // Approximation
  template<typename T> static Complex<T> computeOnComplex(const std::complex<T> c, const std::complex<T> d, Preferences::ComplexFormat complexFormat);
  template<typename T> static MatrixComplex<T> computeOnMatrixAndComplex(const MatrixComplex<T> m, const std::complex<T> c, Preferences::ComplexFormat complexFormat) {
    return ApproximationHelper::ElementWiseOnMatrixAndComplex(m, c, complexFormat, computeOnComplex<T>);
  }
};

class Division final : public ExpressionTwoChildren<Division, DivisionNode> {
public:
  using ExpressionBuilder::ExpressionBuilder;
  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
};

}

#endif
