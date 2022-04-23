#include <poincare/square_root.h>
#include <poincare/addition.h>
#include <poincare/division.h>
#include <poincare/layout_helper.h>
#include <poincare/nth_root_layout.h>
#include <poincare/power.h>
#include <poincare/serialization_helper.h>
#include <poincare/sign_function.h>
#include <poincare/subtraction.h>
#include <poincare/undefined.h>

#include <assert.h>
#include <cmath>
#include <ion.h>

namespace Poincare {

constexpr Expression::FunctionHelper SquareRoot::s_functionHelper;

int SquareRootNode::numberOfChildren() const { return SquareRoot::s_functionHelper.numberOfChildren(); }

Layout SquareRootNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return NthRootLayout::Builder(childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits));
}

int SquareRootNode::serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, SquareRoot::s_functionHelper.name());
}

template<typename T>
Complex<T> SquareRootNode::computeOnComplex(const std::complex<T> c, Preferences::ComplexFormat, Preferences::AngleUnit angleUnit) {
  std::complex<T> result = std::sqrt(c);
  return Complex<T>::Builder(ApproximationHelper::NeglectRealOrImaginaryPartIfNeglectable(result, std::complex<T>(std::log(std::abs(c)), std::arg(c))));
}

Expression SquareRootNode::shallowReduce(ReductionContext reductionContext) {
  return SquareRoot(this).shallowReduce(reductionContext);
}

Expression SquareRoot::shallowReduce(ExpressionNode::ReductionContext reductionContext) {
  {
    Expression e = Expression::defaultShallowReduce();
    if (e.isUndefined()) {
      return e;
    }
  }
  Expression child = childAtIndex(0);
  if (child.deepIsMatrix(reductionContext.context())) {
    return replaceWithUndefinedInPlace();
  }

  // Test if we are in a case like sqrt(a +- sqrt(b)) where a and b are rational
  if (child.type() == ExpressionNode::Type::Addition && child.numberOfChildren() == 2) {
    bool subtraction = child.numberOfChildren() == 2 && child.childAtIndex(1).type() == ExpressionNode::Type::Multiplication && child.childAtIndex(1).numberOfChildren() == 2 && child.childAtIndex(1).childAtIndex(0).type() == ExpressionNode::Type::Rational && child.childAtIndex(1).childAtIndex(0).sign(reductionContext.context()) == ExpressionNode::Sign::Negative;
    Expression a = child.childAtIndex(0);
    Expression b = subtraction && child.childAtIndex(1).childAtIndex(0).convert<Rational>().isMinusOne() ? child.childAtIndex(1).childAtIndex(1) : child.childAtIndex(1);
    // If root have been simplified in x * sqrt(y), we say that b is sqrt(x^2 * y)
    if (b.type() == ExpressionNode::Type::Multiplication && b.numberOfChildren() == 2 && b.childAtIndex(0).type() == ExpressionNode::Type::Rational && b.childAtIndex(0).type() == ExpressionNode::Type::Rational && b.childAtIndex(1).type() == ExpressionNode::Type::Power && b.childAtIndex(1).childAtIndex(0).type() == ExpressionNode::Type::Rational && b.childAtIndex(1).childAtIndex(1).type() == ExpressionNode::Type::Rational && b.childAtIndex(1).childAtIndex(1).convert<Rational>().isHalf()) {
      b = Power::Builder(Multiplication::Builder(Power::Builder(b.childAtIndex(0).clone(), Rational::Builder(2, 1)).shallowReduce(reductionContext), b.childAtIndex(1).childAtIndex(0).clone()).shallowReduce(reductionContext), Rational::Builder(1, 2));
    }
    if (a.type() == ExpressionNode::Type::Rational && b.type() == ExpressionNode::Type::Power && b.childAtIndex(0).type() == ExpressionNode::Type::Rational && b.childAtIndex(1).type() == ExpressionNode::Type::Rational && b.childAtIndex(1).convert<Rational>().isHalf()) {
      Expression a0 = a.clone();
      Expression a1 = b.childAtIndex(0).clone();
      // There is a simplification (wikipedia)
      Expression rr = Subtraction::Builder(Power::Builder(a0, Rational::Builder(2, 1)).shallowReduce(reductionContext), a1).shallowReduce(reductionContext);
      if (a0.sign(reductionContext.context()) != ExpressionNode::Sign::Negative && rr.sign(reductionContext.context()) != ExpressionNode::Sign::Negative) {
        Expression r = Power::Builder(rr, Rational::Builder(1, 2)).shallowReduce(reductionContext);
        if (r.type() == ExpressionNode::Type::Rational) {
          Expression c = Division::Builder(Addition::Builder(a0.clone(), r.clone()).shallowReduce(reductionContext), Rational::Builder(2, 1)).shallowReduce(reductionContext);
          Expression d = Division::Builder(Subtraction::Builder(a0.clone(), r.clone()).shallowReduce(reductionContext), Rational::Builder(2, 1)).shallowReduce(reductionContext);

          Expression result;
          if (subtraction) {
            result = Subtraction::Builder(Power::Builder(c, Rational::Builder(1, 2)).shallowReduce(reductionContext), Power::Builder(d, Rational::Builder(1, 2)).shallowReduce(reductionContext));
          } else {
            result = Addition::Builder(Power::Builder(c, Rational::Builder(1, 2)).shallowReduce(reductionContext), Power::Builder(d, Rational::Builder(1, 2)).shallowReduce(reductionContext));
          }

          replaceWithInPlace(result);
          return result.shallowReduce(reductionContext);
        }
      }
    }
  }

  Power p = Power::Builder(child, Rational::Builder(1, 2));
  replaceWithInPlace(p);
  return p.shallowReduce(reductionContext);
}

}
