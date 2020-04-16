#ifndef POINCARE_DIVISION_H
#define POINCARE_DIVISION_H

#include <poincare/expression.h>
#include <poincare/approximation_helper.h>

namespace Poincare {

class Division;

class DivisionNode /*final*/ : public ExpressionNode {
template<int T>
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
  Type type() const override { return Type::Division; }
  int polynomialDegree(Context * context, const char * symbolName) const override;
  Expression getUnit() const override { assert(false); return ExpressionNode::getUnit(); }

  // Approximation
  virtual Evaluation<float> approximate(SinglePrecision p, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::MapReduce<float>(
        this, context, complexFormat, angleUnit, compute<float>,
        computeOnComplexAndMatrix<float>, computeOnMatrixAndComplex<float>,
        computeOnMatrices<float>);
  }
  virtual Evaluation<double> approximate(DoublePrecision p, Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const override {
    return ApproximationHelper::MapReduce<double>(
        this, context, complexFormat, angleUnit, compute<double>,
        computeOnComplexAndMatrix<double>, computeOnMatrixAndComplex<double>,
        computeOnMatrices<double>);
  }

  // Layout
  bool childNeedsSystemParenthesesAtSerialization(const TreeNode * child) const override;
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;

  // Simplification
  Expression shallowReduce(ReductionContext reductionContext) override;
  LayoutShape leftLayoutShape() const override { return LayoutShape::Fraction; };

private:
  // Approximation
  template<typename T> static Complex<T> compute(const std::complex<T> c, const std::complex<T> d, Preferences::ComplexFormat complexFormat);
  template<typename T> static MatrixComplex<T> computeOnMatrixAndComplex(const MatrixComplex<T> m, const std::complex<T> c, Preferences::ComplexFormat complexFormat) {
    return ApproximationHelper::ElementWiseOnMatrixComplexAndComplex(m, c, complexFormat, compute<T>);
  }
  template<typename T> static MatrixComplex<T> computeOnComplexAndMatrix(const std::complex<T> c, const MatrixComplex<T> n, Preferences::ComplexFormat complexFormat);
  template<typename T> static MatrixComplex<T> computeOnMatrices(const MatrixComplex<T> m, const MatrixComplex<T> n, Preferences::ComplexFormat complexFormat);
};

class Division final : public Expression {
public:
  Division(const DivisionNode * n) : Expression(n) {}
  static Division Builder() { return TreeHandle::FixedArityBuilder<Division, DivisionNode>(); }
  static Division Builder(Expression numerator, Expression denominator) { return TreeHandle::FixedArityBuilder<Division, DivisionNode>({numerator, denominator}); }

  Expression shallowReduce(ExpressionNode::ReductionContext reductionContext);
};

}

#endif
