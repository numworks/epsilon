#include <poincare/sum.h>
#include <poincare/addition.h>
#include <poincare/sum_layout.h>
#include <poincare/layout_helper.h>
#include <poincare/parametered_expression_helper.h>
#include <poincare/serialization_helper.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
}
#include <cmath>

namespace Poincare {

constexpr Expression::FunctionHelper Sum::s_functionHelper;

Expression SumNode::replaceUnknown(const Symbol & symbol) {
  return ParameteredExpressionHelper::ReplaceUnknownInExpression(Sum(this), symbol);
}

Layout SumNode::createSequenceLayout(Layout argumentLayout, Layout symbolLayout, Layout subscriptLayout, Layout superscriptLayout) const {
  return SumLayout::Builder(argumentLayout, symbolLayout, subscriptLayout, superscriptLayout);
}

int SumNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, Sum::s_functionHelper.name());
}

template<typename T>
Evaluation<T> SumNode::templatedApproximateWithNextTerm(Evaluation<T> a, Evaluation<T> b, Preferences::ComplexFormat complexFormat) const {
  if (a.type() == EvaluationNode<T>::Type::Complex && b.type() == EvaluationNode<T>::Type::Complex) {
    Complex<T> c = static_cast<Complex<T>&>(a);
    Complex<T> d = static_cast<Complex<T>&>(b);
    return Complex<T>::Builder(c.stdComplex()+d.stdComplex());
  }
  if (a.type() == EvaluationNode<T>::Type::Complex) {
    Complex<T> c = static_cast<Complex<T> &>(a);
    assert(b.type() == EvaluationNode<T>::Type::MatrixComplex);
    MatrixComplex<T> m = static_cast<MatrixComplex<T> &>(b);
    return AdditionNode::computeOnComplexAndMatrix(c.stdComplex(), m, complexFormat);
  }
  assert(a.type() == EvaluationNode<T>::Type::MatrixComplex);
  assert(b.type() == EvaluationNode<T>::Type::MatrixComplex);
  MatrixComplex<T> m = static_cast<MatrixComplex<T>&>(a);
  MatrixComplex<T> n = static_cast<MatrixComplex<T>&>(b);
  return AdditionNode::computeOnMatrices<T>(m, n, complexFormat);
}

Sum Sum::Builder(Expression child0, Symbol child1, Expression child2, Expression child3) {
  void * bufferNode = TreePool::sharedPool()->alloc(sizeof(SumNode));
  SumNode * node = new (bufferNode) SumNode();
  TreeHandle h = TreeHandle::BuildWithBasicChildren(node, ArrayBuilder<Expression>(child0, child1, child2, child3).array(), 4);
  return static_cast<Sum &>(h);
}

}
