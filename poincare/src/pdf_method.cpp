#include <poincare/pdf_method.h>
#include <poincare/infinity.h>
#include <poincare/integer.h>
#include <poincare/rational.h>

namespace Poincare {

Expression PDFMethod::shallowReduce(Expression * abscissae, const Distribution * distribution, Expression * parameters, ExpressionNode::ReductionContext reductionContext, Expression * expression) const {
  Expression x = abscissae[0];

  if (x.type() == ExpressionNode::Type::Infinity) {
    Expression result = Rational::Builder(0);
    expression->replaceWithInPlace(result);
    return result;
  }

  if (x.type() != ExpressionNode::Type::Rational) {
    return *expression;
  }

  if (static_cast<Rational &>(x).isNegative()
      && (distribution->hasType(Distribution::Type::Binomial)
          | distribution->hasType(Distribution::Type::Poisson)
          | distribution->hasType(Distribution::Type::Geometric))) {
    Expression result = Rational::Builder(0);
    expression->replaceWithInPlace(result);
    return result;
  }

  if (!distribution->isContinuous()) {
    Rational r = static_cast<Rational &>(x);
    IntegerDivision div = Integer::Division(r.signedIntegerNumerator(), r.integerDenominator());
    assert(!div.quotient.isOverflow());
    Expression result = Rational::Builder(div.quotient);
    x.replaceWithInPlace(result);
  }

  return *expression;
}

}
