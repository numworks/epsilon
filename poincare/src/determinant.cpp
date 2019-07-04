#include <poincare/determinant.h>
#include <poincare/addition.h>
#include <poincare/matrix.h>
#include <poincare/layout_helper.h>
#include <poincare/rational.h>
#include <poincare/serialization_helper.h>
#include <poincare/subtraction.h>
extern "C" {
#include <assert.h>
}
#include <cmath>

namespace Poincare {

constexpr Expression::FunctionHelper Determinant::s_functionHelper;

int DeterminantNode::numberOfChildren() const { return Determinant::s_functionHelper.numberOfChildren(); }

Layout DeterminantNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(Determinant(this), floatDisplayMode, numberOfSignificantDigits, Determinant::s_functionHelper.name());
}

int DeterminantNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, Determinant::s_functionHelper.name());
}

template<typename T>
Evaluation<T> DeterminantNode::templatedApproximate(Context * context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit) const {
  Evaluation<T> input = childAtIndex(0)->approximate(T(), context, complexFormat, angleUnit);
  return Complex<T>::Builder(input.determinant());
}

Expression DeterminantNode::shallowReduce(ReductionContext reductionContext) {
  return Determinant(this).shallowReduce(reductionContext);
}

Expression Determinant::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  {
    Expression e = Expression::defaultShallowReduce();
    if (e.isUndefined()) {
      return e;
    }
  }
  Expression c0 = childAtIndex(0);
  // det(A) = A if A is not a matrix
  if (!SortedIsMatrix(c0, reductionContext.context())) {
    replaceWithInPlace(c0);
    return c0;
  }
  if (c0.type() == ExpressionNode::Type::Matrix) {
    Matrix m0 = static_cast<Matrix &>(c0);
    int dim = m0.numberOfRows();
    if (dim != m0.numberOfColumns()) {
      // Determinant is for square matrices
      return replaceWithUndefinedInPlace();
    }
    Expression result;
    if (dim == 1) {
      // Determinant of [[a]] is a
      result = m0.childAtIndex(0);
    } else if (dim == 2) {
      /*                |a b|
       * Determinant of |c d| is ad-bc   */
      Multiplication ad = Multiplication::Builder(m0.matrixChild(0,0), m0.matrixChild(1,1));
      Multiplication bc = Multiplication::Builder(m0.matrixChild(0,1), m0.matrixChild(1,0));
      result = Subtraction::Builder(ad, bc);
      ad.shallowReduce(reductionContext);
      bc.shallowReduce(reductionContext);
    } else if (dim == 3) {
      /*                |a b c|
       * Determinant of |d e f| is aei+bfg+cdh-ceg-bdi-afh
       *                |g h i|                             */
      Expression a = m0.matrixChild(0,0);
      Expression b = m0.matrixChild(0,1);
      Expression c = m0.matrixChild(0,2);
      Expression d = m0.matrixChild(1,0);
      Expression e = m0.matrixChild(1,1);
      Expression f = m0.matrixChild(1,2);
      Expression g = m0.matrixChild(2,0);
      Expression h = m0.matrixChild(2,1);
      Expression i = m0.matrixChild(2,2);
      constexpr int additionChildrenCount = 6;
      Expression additionChildren[additionChildrenCount] = {
        Multiplication::Builder(a.clone(), e.clone(), i.clone()),
        Multiplication::Builder(b.clone(), f.clone(), g.clone()),
        Multiplication::Builder(c.clone(), d.clone(), h.clone()),
        Multiplication::Builder(Rational::Builder(-1), c, e, g),
        Multiplication::Builder(Rational::Builder(-1), b, d, i),
        Multiplication::Builder(Rational::Builder(-1), a, f, h)};
      result = Addition::Builder(additionChildren, additionChildrenCount);
      for (int i = 0; i < additionChildrenCount; i++) {
        additionChildren[i].shallowReduce(reductionContext);
      }
    }
    if (!result.isUninitialized()) {
      replaceWithInPlace(result);
      return result.shallowReduce(reductionContext);
    }
  }
  return *this;
}

}
