#include "trigonometry_list_controller.h"
#include "../app.h"
#include <apps/shared/poincare_helpers.h>
#include <poincare_expressions.h>
#include <poincare/trigonometry.h>
#include <poincare/layout_helper.h>

using namespace Poincare;

namespace Calculation {

constexpr char TrigonometryListController::k_symbol[];

void TrigonometryListController::setExpression(Expression e) {
  IllustratedExpressionsListController::setExpression(e);

  Poincare::Preferences * preferences = Preferences::sharedPreferences();
  Context * context = App::app()->localContext();
  size_t index = 0;

  Expression unit;
  Shared::PoincareHelpers::ReduceAndRemoveUnit(&e, context, ExpressionNode::ReductionTarget::User, &unit);
  assert(unit.isUninitialized() || static_cast<Unit &>(unit).representative()->dimensionVector() == Unit::AngleRepresentative::Default().dimensionVector());

  // Convert angle in radian once for all
  if (unit.isUninitialized() && preferences->angleUnit() != Preferences::AngleUnit::Radian) {
    e = Multiplication::Builder(e, Trigonometry::UnitConversionFactor(preferences->angleUnit(), Preferences::AngleUnit::Radian));
  }

  Poincare::Preferences preferencesRadian = *preferences;
  preferencesRadian.setAngleUnit(Preferences::AngleUnit::Radian);

  // Compute angle modulus 2π
  Expression twoPi = Multiplication::Builder(Rational::Builder(2), Poincare::Constant::Builder("π"));
  // Use the reduction of frac part to compute mod 1 on rationals
  e = Multiplication::Builder(FracPart::Builder(Division::Builder(e, twoPi.clone())), twoPi.clone());
  Shared::PoincareHelpers::CloneAndReduce(&e, context, ExpressionNode::ReductionTarget::User);
  // If frac part is still there, the exact angle is probably not interesting
  if (e.recursivelyMatches([] (const Expression e, Context * context) { return e.type() == ExpressionNode::Type::FracPart; })) {
    e = Shared::PoincareHelpers::Approximate<double>(e, context);
  }
  m_layouts[index] = LayoutHelper::String("θ");
  Expression radian = Unit::Builder(Unit::k_angleRepresentatives + Unit::k_radianRepresentativeIndex);
  Expression degrees = Unit::Builder(Unit::k_angleRepresentatives + Unit::k_degreeRepresentativeIndex);
  Expression inRadian = Multiplication::Builder(e, radian);
  m_exactLayouts[index] = getLayoutFromExpression(inRadian, context, &preferencesRadian);
  m_approximatedLayouts[index] = getLayoutFromExpression(UnitConvert::Builder(inRadian, degrees), context, &preferencesRadian);
  index++;

  appendLine(index++, "cos(θ)", Cosine::Builder(e), context, &preferencesRadian);
  appendLine(index++, "sin(θ)", Sine::Builder(e), context, &preferencesRadian);
  appendLine(index++, "tan(θ)", Tangent::Builder(e), context, &preferencesRadian);

  // Set illustration
  float angle = Shared::PoincareHelpers::ApproximateToScalar<float>(e, context);
  m_model.setAngle(angle);
  setShowIllustration(true);
}

void TrigonometryListController::appendLine(int index, const char * formula, Poincare::Expression expression, Poincare::Context * context, Poincare::Preferences * preferences) {
  m_layouts[index] = LayoutHelper::String(formula);
  Layout exact = getLayoutFromExpression(expression, context, preferences);
  Layout approximated = getLayoutFromExpression(expression.approximate<double>(context, preferences->complexFormat(), preferences->angleUnit()), context, preferences);
  // Make it editable to have Horiz(CodePoint("-"),CodePoint("1") == String("-1")
  m_exactLayouts[index] = exact.isIdenticalTo(approximated, true) ? Layout() : exact;
  m_approximatedLayouts[index] = approximated;
  index++;
};

KDCoordinate TrigonometryListController::nonMemoizedRowHeight(int j) {
  if (typeAtIndex(j) == k_illustrationCellType) {
    return k_illustrationHeight;
  }
  return IllustratedExpressionsListController::nonMemoizedRowHeight(j);
}

I18n::Message TrigonometryListController::messageAtIndex(int index) {
  if (index == 0) {
    return I18n::Message::AngleInZeroTwoPi;
  }
  return I18n::Message::Default;
}

}
