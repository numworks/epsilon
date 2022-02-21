#include "trigonometry_list_controller.h"
#include "../app.h"
#include <poincare_nodes.h>
#include "../../shared/poincare_helpers.h"

using namespace Poincare;

namespace Calculation {

static constexpr int s_fullCircle[] = {
  360,
  2,
  400
};

Poincare::Constant toConstant(Expression e) {
  return static_cast<Poincare::Constant &>(e);
}

void TrigonometryListController::setExpression(Expression e) {
  assert(e.type() == ExpressionNode::Type::Cosine || e.type() == ExpressionNode::Type::Sine);

  Poincare::Context * context = App::app()->localContext();
  Poincare::Preferences * preferences = Poincare::Preferences::sharedPreferences();
  Preferences::AngleUnit angleUnit = preferences->angleUnit();

  Expression angleExpression = e.childAtIndex(0);

  Shared::PoincareHelpers::Reduce(&angleExpression, context, Poincare::ExpressionNode::ReductionTarget::SystemForAnalysis);

  if ((angleUnit == Preferences::AngleUnit::Radian
        && angleExpression.type() == ExpressionNode::Type::Multiplication
        && angleExpression.numberOfChildren() == 2
        && angleExpression.childAtIndex(1).type() == ExpressionNode::Type::Constant
        && toConstant(angleExpression.childAtIndex(1)).isPi()
        && angleExpression.childAtIndex(0).type() == ExpressionNode::Type::Rational)
      || ((angleUnit == Preferences::AngleUnit::Degree || angleUnit == Preferences::AngleUnit::Gradian)
        && angleExpression.type() == ExpressionNode::Type::Rational)) {

    Expression extracted = angleUnit == Preferences::AngleUnit::Radian ? angleExpression.childAtIndex(0) : angleExpression;
    Rational r = static_cast<Rational &>(extracted);

    Integer denominator = Integer::Multiplication(r.integerDenominator(), Integer(s_fullCircle[(int) angleUnit]));
    IntegerDivision division = Integer::Division(r.signedIntegerNumerator(), denominator);

    Integer remainder = division.remainder;

    Expression newAngle;
    Integer rDenominator = r.integerDenominator();
    Rational newCoefficient = Rational::Builder(remainder, rDenominator);
    if (angleUnit == Preferences::AngleUnit::Radian) {
      angleExpression = Multiplication::Builder(newCoefficient, angleExpression.childAtIndex(1));
    } else {
      angleExpression = newCoefficient;
    }
  }

  IllustratedListController::setExpression(angleExpression);

  // Fill calculation store
  m_calculationStore.push("sin(θ)", context, CalculationHeight);
  m_calculationStore.push("cos(θ)", context, CalculationHeight);
  m_calculationStore.push("θ", context, CalculationHeight);

  // Set trigonometry illustration
  float angle = Shared::PoincareHelpers::ApproximateToScalar<float>(m_calculationStore.calculationAtIndex(0)->approximateOutput(context, Calculation::NumberOfSignificantDigits::Maximal), context);
  m_model.setAngle(angle);
}

}
