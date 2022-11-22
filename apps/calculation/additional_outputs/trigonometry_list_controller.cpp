#include "trigonometry_list_controller.h"
#include "../app.h"
#include <apps/shared/poincare_helpers.h>
#include <poincare_expressions.h>
#include <poincare/trigonometry.h>
#include <poincare/layout_helper.h>

using namespace Poincare;

namespace Calculation {

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

  Expression theta = Symbol::Builder(k_symbol);
  appendLine(index++, Cosine::Builder(theta.clone()), Cosine::Builder(e), context, &preferencesRadian);
  appendLine(index++, Sine::Builder(theta.clone()), Sine::Builder(e), context, &preferencesRadian);
  appendLine(index++, Tangent::Builder(theta), Tangent::Builder(e), context, &preferencesRadian);

  // Set illustration
  float angle = Shared::PoincareHelpers::ApproximateToScalar<float>(e, context);
  assert(std::isfinite(angle));
  m_model.setAngle(angle);
  setShowIllustration(true);
}

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
