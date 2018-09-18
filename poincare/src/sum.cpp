#include <poincare/sum.h>
#include <poincare/addition.h>
#include <poincare/sum_layout.h>

extern "C" {
#include <assert.h>
#include <stdlib.h>
}
#include <cmath>

namespace Poincare {


Layout SumNode::createSequenceLayout(Layout argumentLayout, Layout subscriptLayout, Layout superscriptLayout) const {
  return SumLayout(argumentLayout, subscriptLayout, superscriptLayout);
}

template<typename T>
Evaluation<T> SumNode::templatedApproximateWithNextTerm(Evaluation<T> a, Evaluation<T> b) const {
  if (a.type() == EvaluationNode<T>::Type::Complex && b.type() == EvaluationNode<T>::Type::Complex) {
    Complex<T> c = static_cast<Complex<T>&>(a);
    Complex<T> d = static_cast<Complex<T>&>(b);
    return Complex<T>(c.stdComplex()+d.stdComplex());
  }
  if (a.type() == EvaluationNode<T>::Type::Complex) {
    Complex<T> c = static_cast<Complex<T> &>(a);
    assert(b.type() == EvaluationNode<T>::Type::MatrixComplex);
    MatrixComplex<T> m = static_cast<MatrixComplex<T> &>(b);
    return AdditionNode::computeOnComplexAndMatrix(c.stdComplex(), m);
  }
  assert(a.type() == EvaluationNode<T>::Type::MatrixComplex);
  assert(b.type() == EvaluationNode<T>::Type::MatrixComplex);
  MatrixComplex<T> m = static_cast<MatrixComplex<T>&>(a);
  MatrixComplex<T> n = static_cast<MatrixComplex<T>&>(b);
  return AdditionNode::computeOnMatrices<T>(m, n);
}

Sum::Sum() : Expression(TreePool::sharedPool()->createTreeNode<SumNode>()) {}

}
