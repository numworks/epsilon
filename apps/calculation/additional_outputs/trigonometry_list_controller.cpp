#include "trigonometry_list_controller.h"

#include <apps/shared/expression_display_permissions.h>
#include <apps/shared/poincare_helpers.h>
#include <poincare/layout_helper.h>
#include <poincare/trigonometry.h>
#include <poincare_expressions.h>

#include "../app.h"

using namespace Poincare;

namespace Calculation {

void TrigonometryListController::setExactAndApproximateExpression(
    Poincare::Expression exactAngle, Poincare::Expression approximateAngle) {
  IllustratedExpressionsListController::setExactAndApproximateExpression(
      exactAngle, approximateAngle);

  Preferences* preferences = Preferences::sharedPreferences;
  Preferences::AngleUnit userAngleUnit = preferences->angleUnit();
  Context* context = App::app()->localContext();
  size_t index = 0;

  Expression period = Multiplication::Builder(
      Rational::Builder(2),
      Trigonometry::PiExpressionInAngleUnit(userAngleUnit));

  Expression simplifiedAngle;
  Expression unit;

  // If the approximate angle is in [-π, π], set it in [0, 2π]
  if (!approximateAngle.isUninitialized() &&
      approximateAngle.isPositive(context) == TrinaryBoolean::False) {
    approximateAngle = Addition::Builder(period.clone(), approximateAngle);
  }

  Shared::PoincareHelpers::CloneAndReduceAndRemoveUnit(
      &exactAngle, context, ReductionTarget::User, &unit);

  // Set exact angle in [0, 2π]
  if (!exactAngle.isUndefined()) {
    if (!unit.isUninitialized()) {
      assert(unit.isPureAngleUnit() &&
             static_cast<Unit&>(unit).representative() ==
                 Unit::k_angleRepresentatives +
                     Unit::k_radianRepresentativeIndex);
      /* After a reduction, all angle units are converted to radians, so we
       * convert e again here to fit the angle unit that will be used in
       * reductions below. */
      exactAngle = Multiplication::Builder(
          exactAngle, Trigonometry::UnitConversionFactor(
                          Preferences::AngleUnit::Radian, userAngleUnit));
    }

    // Use the reduction of frac part to compute mod 1 on rationals
    simplifiedAngle = Multiplication::Builder(
        FracPart::Builder(Division::Builder(exactAngle, period.clone())),
        period.clone());
    Shared::PoincareHelpers::CloneAndSimplify(&simplifiedAngle, context,
                                              ReductionTarget::User);
  }

  /* Approximate the angle if:
   * - The reduction failed
   * - The fractional part could not be reduced (because the angle is not a
   * multiple of pi)
   * - Displaying the exact expression is forbidden. */
  if (simplifiedAngle.isUninitialized() ||
      simplifiedAngle.recursivelyMatches(
          [](const Expression e, Context* context) {
            return e.type() == ExpressionNode::Type::FracPart;
          }) ||
      Shared::ExpressionDisplayPermissions::ShouldOnlyDisplayApproximation(
          exactAngle, simplifiedAngle, approximateAngle, context)) {
    if (approximateAngle.isUninitialized()) {
      assert(!exactAngle.isUndefined());
      /* In case of direct trigonometry, the approximate expression of the angle
       * is not yet computed, so it needs to be computed here.
       * Do not approximate the FracPart, which could lead to truncation error
       * for large angles (e.g. frac(1e17/2pi) = 0). Instead find the angle with
       * the same sine and cosine. */
      approximateAngle =
          ArcCosine::Builder(Cosine::Builder(exactAngle.clone()));
      /* acos has its values in [0,π[, use the sign of the sine to find the
       * right semicircle. */
      if (Shared::PoincareHelpers::ApproximateToScalar<double>(
              Sine::Builder(exactAngle), context, preferences) < 0) {
        approximateAngle =
            Subtraction::Builder(period.clone(), approximateAngle);
      }
    }
    approximateAngle = Shared::PoincareHelpers::Approximate<double>(
        approximateAngle, context, preferences);
    exactAngle = approximateAngle;
    m_isStrictlyEqual[index] = false;
  } else {
    exactAngle = simplifiedAngle;
    m_isStrictlyEqual[index] = true;
  }

  m_layouts[index] = LayoutHelper::String("θ");

  Expression withAngleUnit = Multiplication::Builder(
      exactAngle.clone(),
      Unit::Builder(
          UnitNode::AngleRepresentative::DefaultRepresentativeForAngleUnit(
              userAngleUnit)));

  Expression radians = Unit::Builder(Unit::k_angleRepresentatives +
                                     Unit::k_radianRepresentativeIndex);
  m_exactLayouts[index] = getLayoutFromExpression(
      UnitConvert::Builder(withAngleUnit.clone(), radians), context,
      preferences);

  Expression degrees = Unit::Builder(Unit::k_angleRepresentatives +
                                     Unit::k_degreeRepresentativeIndex);
  m_approximatedLayouts[index] = getLayoutFromExpression(
      UnitConvert::Builder(withAngleUnit.clone(), degrees), context,
      preferences);

  Expression theta = Symbol::Builder(k_symbol);
  setLineAtIndex(++index, Cosine::Builder(theta),
                 Cosine::Builder(exactAngle.clone()), context, preferences);
  updateIsStrictlyEqualAtIndex(index, context);
  setLineAtIndex(++index, Sine::Builder(theta),
                 Sine::Builder(exactAngle.clone()), context, preferences);
  updateIsStrictlyEqualAtIndex(index, context);
  setLineAtIndex(++index, Tangent::Builder(theta),
                 Tangent::Builder(exactAngle.clone()), context, preferences);
  updateIsStrictlyEqualAtIndex(index, context);

  // Set illustration
  /* m_model ask for a float angle but we compute the angle in double and then
   * cast it to float because approximation in float can overflow during the
   * computation. The angle should be between 0 and 2*pi so the approximation in
   * double is castable in float. */
  float angle =
      static_cast<float>(Shared::PoincareHelpers::ApproximateToScalar<double>(
          approximateAngle.isUninitialized() ? exactAngle : approximateAngle,
          context));
  // Convert angle to radians
  if (userAngleUnit != Preferences::AngleUnit::Radian) {
    angle = angle * M_PI / Trigonometry::PiInAngleUnit(userAngleUnit);
  }
  assert(std::isfinite(angle));
  assert(0 <= angle && angle < 2 * M_PI + Float<float>::EpsilonLax());
  m_model.setAngle(angle);
  setShowIllustration(true);
}

void TrigonometryListController::updateIsStrictlyEqualAtIndex(
    int index, Context* context) {
  if (m_approximatedLayouts[index].isUninitialized()) {
    /* Only one layout is displayed, so there is no equal sign. */
    return;
  }
  char approximateBuffer[::Constant::MaxSerializedExpressionSize];
  m_approximatedLayouts[index].serializeForParsing(
      approximateBuffer, ::Constant::MaxSerializedExpressionSize);
  if (strcmp(approximateBuffer, Undefined::Name()) == 0) {
    // Hide exact result if approximation is undef (e.g tan(1.5707963267949))
    m_exactLayouts[index] = Layout();
    return;
  }
  char exactBuffer[::Constant::MaxSerializedExpressionSize];
  m_exactLayouts[index].serializeForParsing(
      exactBuffer, ::Constant::MaxSerializedExpressionSize);
  assert(strcmp(exactBuffer, approximateBuffer) != 0);
  m_isStrictlyEqual[index] = Expression::ExactAndApproximateExpressionsAreEqual(
      Expression::Parse(exactBuffer, context),
      Expression::Parse(approximateBuffer, context));
}

void TrigonometryListController::fillCellForRow(Escher::HighlightCell* cell,
                                                int row) {
  if (typeAtRow(row) == k_expressionCellType) {
    int expressionIndex = row - showIllustration();
    assert(0 <= expressionIndex && expressionIndex < k_numberOfExpressionRows);
    static_cast<AdditionnalResultCell*>(cell)
        ->label()
        ->setExactAndApproximateAreStriclyEqual(
            m_isStrictlyEqual[expressionIndex]);
  }
  return IllustratedExpressionsListController::fillCellForRow(cell, row);
}

KDCoordinate TrigonometryListController::nonMemoizedRowHeight(int row) {
  if (typeAtRow(row) == k_illustrationCellType) {
    return k_illustrationHeight;
  }
  return IllustratedExpressionsListController::nonMemoizedRowHeight(row);
}

I18n::Message TrigonometryListController::messageAtIndex(int index) {
  if (index == 0) {
    return I18n::Message::AngleInZeroTwoPi;
  }
  return I18n::Message::Default;
}

}  // namespace Calculation
