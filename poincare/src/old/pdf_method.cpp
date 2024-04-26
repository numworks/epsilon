#include <poincare/old/infinity.h>
#include <poincare/old/integer.h>
#include <poincare/old/pdf_method.h>
#include <poincare/old/rational.h>

namespace Poincare {

OExpression PDFMethod::shallowReduce(OExpression *abscissae,
                                     const Distribution *distribution,
                                     OExpression *parameters,
                                     ReductionContext reductionContext,
                                     OExpression *expression) const {
  OExpression x = abscissae[0];

  if (x.otype() == ExpressionNode::Type::Infinity) {
    OExpression result = Rational::Builder(0);
    expression->replaceWithInPlace(result);
    return result;
  }

  if (x.otype() != ExpressionNode::Type::Rational) {
    return *expression;
  }

  if (static_cast<Rational &>(x).isNegative() &&
      (distribution->hasType(Distribution::Type::Binomial) ||
       distribution->hasType(Distribution::Type::Poisson) ||
       distribution->hasType(Distribution::Type::Geometric) ||
       distribution->hasType(Distribution::Type::Hypergeometric))) {
    OExpression result = Rational::Builder(0);
    expression->replaceWithInPlace(result);
    return result;
  }

  if (!distribution->isContinuous()) {
    Rational r = static_cast<Rational &>(x);
    IntegerDivision div =
        Integer::Division(r.signedIntegerNumerator(), r.integerDenominator());
    assert(!div.quotient.isOverflow());
    OExpression result = Rational::Builder(div.quotient);
    x.replaceWithInPlace(result);
  }

  return *expression;
}

}  // namespace Poincare
