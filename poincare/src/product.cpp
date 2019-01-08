#include <poincare/product.h>
#include <poincare/multiplication.h>
#include <poincare/parametered_expression_helper.h>
#include <poincare/product_layout.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
}
#include <cmath>

namespace Poincare {

constexpr Expression::FunctionHelper Product::s_functionHelper;

Expression ProductNode::replaceUnknown(const Symbol & symbol) {
  return ParameteredExpressionHelper::ReplaceUnknownInExpression(Product(this), symbol);
}

Layout ProductNode::createSequenceLayout(Layout argumentLayout, Layout symbolLayout, Layout subscriptLayout, Layout superscriptLayout) const {
  return ProductLayout(argumentLayout, symbolLayout, subscriptLayout, superscriptLayout);
}

int ProductNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, Product::s_functionHelper.name());
}

template<typename T>
Evaluation<T> ProductNode::templatedApproximateWithNextTerm(Evaluation<T> a, Evaluation<T> b) const {
  if (a.type() == EvaluationNode<T>::Type::Complex && b.type() == EvaluationNode<T>::Type::Complex) {
    Complex<T> c = static_cast<Complex<T>&>(a);
    Complex<T> d = static_cast<Complex<T>&>(b);
    return Complex<T>(c.stdComplex()*d.stdComplex());
  }
  if (a.type() == EvaluationNode<T>::Type::Complex) {
    Complex<T> c = static_cast<Complex<T> &>(a);
    assert(b.type() == EvaluationNode<T>::Type::MatrixComplex);
    MatrixComplex<T> m = static_cast<MatrixComplex<T> &>(b);
    return MultiplicationNode::computeOnComplexAndMatrix(c.stdComplex(), m);
  }
  assert(a.type() == EvaluationNode<T>::Type::MatrixComplex);
  assert(b.type() == EvaluationNode<T>::Type::MatrixComplex);
  MatrixComplex<T> m = static_cast<MatrixComplex<T>&>(a);
  MatrixComplex<T> n = static_cast<MatrixComplex<T>&>(b);
  return MultiplicationNode::computeOnMatrices<T>(m, n);
}

}
