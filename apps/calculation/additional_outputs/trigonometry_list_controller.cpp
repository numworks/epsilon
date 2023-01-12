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
  Shared::PoincareHelpers::ReduceAndRemoveUnit(&e, context, ReductionTarget::User, &unit);
  assert(unit.isUninitialized() || static_cast<Unit &>(unit).representative()->dimensionVector() == Unit::AngleRepresentative::Default().dimensionVector());

  // Convert angle in radian once for all
  if (unit.isUninitialized() && preferences->angleUnit() != Preferences::AngleUnit::Radian) {
    e = Multiplication::Builder(e, Trigonometry::UnitConversionFactor(preferences->angleUnit(), Preferences::AngleUnit::Radian));
  }

  Poincare::Preferences preferencesRadian = *preferences;
  preferencesRadian.setAngleUnit(Preferences::AngleUnit::Radian);

  // Compute angle modulus 2π
  Expression twoPi = Multiplication::Builder(Rational::Builder(2), Poincare::Constant::PiBuilder());
  // Use the reduction of frac part to compute mod 1 on rationals
  Expression angleReduced = Multiplication::Builder(FracPart::Builder(Division::Builder(e, twoPi.clone())), twoPi.clone());
  Shared::PoincareHelpers::CloneAndReduce(&angleReduced, context, ReductionTarget::User);
  // If frac part is still there, the exact angle is probably not interesting
  if (angleReduced.recursivelyMatches([] (const Expression e, Context * context) { return e.type() == ExpressionNode::Type::FracPart; })) {
    /* Do not approximate the FracPart, which could lead to truncation error
     * for large angles (e.g. frac(1e17/2pi) = 0). Instead find the angle with
     * the same sine and cosine. */
    Expression angleApproximate = ArcCosine::Builder(Cosine::Builder(e));
    angleApproximate = Shared::PoincareHelpers::Approximate<double>(angleApproximate, context, &preferencesRadian);
    /* acos has its values in [0,π[, use the sign of the sine to find the right
     * semicircle. */
    if (Shared::PoincareHelpers::ApproximateToScalar<double>(Sine::Builder(e), context, &preferencesRadian) < 0) {
      angleApproximate = Shared::PoincareHelpers::Approximate<double>(Subtraction::Builder(twoPi.clone(), angleApproximate), context, &preferencesRadian);
    }
    e = angleApproximate;
    m_anglesAreEqual = false;
  } else {
    e = angleReduced;
    m_anglesAreEqual = true;
  }
  m_layouts[index] = LayoutHelper::String("θ");
  Expression radian = Unit::Builder(Unit::k_angleRepresentatives + Unit::k_radianRepresentativeIndex);
  Expression degrees = Unit::Builder(Unit::k_angleRepresentatives + Unit::k_degreeRepresentativeIndex);
  Expression inRadian = Multiplication::Builder(e, radian);
  m_exactLayouts[index] = getLayoutFromExpression(inRadian, context, &preferencesRadian);
  m_approximatedLayouts[index] = getLayoutFromExpression(UnitConvert::Builder(inRadian, degrees), context, &preferencesRadian);
  index++;

  Expression theta = Symbol::Builder(k_symbol);
  setLineAtIndex(index++, Cosine::Builder(theta), Cosine::Builder(e), context, &preferencesRadian);
  setLineAtIndex(index++, Sine::Builder(theta), Sine::Builder(e), context, &preferencesRadian);
  setLineAtIndex(index++, Tangent::Builder(theta), Tangent::Builder(e), context, &preferencesRadian);

  // Set illustration
  float angle = Shared::PoincareHelpers::ApproximateToScalar<float>(e, context);
  assert(std::isfinite(angle));
  m_model.setAngle(angle);
  setShowIllustration(true);
}

void TrigonometryListController::willDisplayCellForIndex(Escher::HighlightCell * cell, int index) {
  if (index == 1) {
    static_cast<ScrollableThreeExpressionsCell*>(cell)->setRightIsStrictlyEqual(m_anglesAreEqual);
  }
  return IllustratedExpressionsListController::willDisplayCellForIndex(cell, index);
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
