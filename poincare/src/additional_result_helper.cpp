#include <apps/shared/expression_display_permissions.h>
#include <apps/shared/poincare_helpers.h>
#include <poincare/additional_result_helper.h>
#include <poincare/k_tree.h>
#include <poincare/new_trigonometry.h>
#include <poincare/old/arc_cosine.h>
#include <poincare/old/cosine.h>
#include <poincare/old/multiplication.h>
#include <poincare/old/sine.h>
#include <poincare/old/subtraction.h>
#include <poincare/src/expression/projection.h>

namespace Poincare {

void AdditionalResultHelper::TrigonometryAngleHelper(
    const UserExpression input, const UserExpression exactOutput,
    const UserExpression approximateOutput, bool directTrigonometry,
    Poincare::Preferences::CalculationPreferences calculationPreferences,
    Internal::ProjectionContext* ctx, UserExpression& exactAngle,
    float* approximatedAngle, bool* angleIsExact) {
  // TODO: Move this in additional_result_helper, use Trees and new Trigonometry
  UserExpression period =
      Trigonometry::AnglePeriodInAngleUnit(ctx->m_angleUnit);
  UserExpression approximateAngle;
  // Find the angle
  if (directTrigonometry) {
    exactAngle = ExtractExactAngleFromDirectTrigo(
        input, exactOutput, ctx->m_context, calculationPreferences);
    approximateAngle = UserExpression();
  } else {
    exactAngle = exactOutput;
    approximateAngle = approximateOutput;
    assert(!approximateAngle.isUninitialized() &&
           !approximateAngle.isUndefined());
    if (approximateAngle.isPositive(ctx->m_context) == OMG::Troolean::False) {
      // If the approximate angle is in [-π, π], set it in [0, 2π]
      approximateAngle = UserExpression::Create(
          KAdd(KA, KB), {.KA = period, .KB = approximateAngle});
    }
  }
  assert(!exactAngle.isUninitialized() && !exactAngle.isUndefined());

  /* Set exact angle in [0, 2π].
   * Use the reduction of frac part to compute modulo. */
  UserExpression simplifiedAngle = UserExpression::Create(
      KMult(KFrac(KDiv(KA, KB)), KB), {.KA = exactAngle, .KB = period});
  Shared::PoincareHelpers::CloneAndSimplify(
      &simplifiedAngle, ctx->m_context,
      {.complexFormat = ctx->m_complexFormat, .angleUnit = ctx->m_angleUnit});
  assert(simplifiedAngle.isUninitialized() || !simplifiedAngle.isUndefined());

  /* Approximate the angle if:
   * - The reduction failed
   * - The fractional part could not be reduced (because the angle is not a
   * multiple of pi)
   * - Displaying the exact expression is forbidden. */
  if (simplifiedAngle.isUninitialized() ||
      simplifiedAngle.deepIsOfType({ExpressionNode::Type::FracPart}) ||
      Shared::ExpressionDisplayPermissions::ShouldOnlyDisplayApproximation(
          exactAngle, simplifiedAngle, approximateAngle, ctx->m_context)) {
    if (directTrigonometry) {
      assert(approximateAngle.isUninitialized());
      /* Do not approximate the FracPart, which could lead to truncation error
       * for large angles (e.g. frac(1e17/2pi) = 0). Instead find the angle with
       * the same sine and cosine. */
      approximateAngle = ArcCosine::Builder(Cosine::Builder(exactAngle));
      /* acos has its values in [0,π[, use the sign of the sine to find the
       * right semicircle. */
      if (Shared::PoincareHelpers::ApproximateToScalar<double>(
              Sine::Builder(exactAngle), ctx->m_context,
              {.complexFormat = ctx->m_complexFormat,
               .angleUnit = ctx->m_angleUnit}) < 0) {
        approximateAngle = Subtraction::Builder(period, approximateAngle);
      }
    }
    assert(!approximateAngle.isUninitialized());
    approximateAngle = Shared::PoincareHelpers::Approximate<double>(
        approximateAngle, ctx->m_context,
        {.complexFormat = ctx->m_complexFormat, .angleUnit = ctx->m_angleUnit});
    exactAngle = approximateAngle;
    *angleIsExact = false;
  } else {
    exactAngle = simplifiedAngle;
  }
  assert(!exactAngle.isUninitialized() && !exactAngle.isUndefined());

  /* m_model ask for a float angle but we compute the angle in double and then
   * cast it to float because approximation in float can overflow during the
   * computation. The angle should be between 0 and 2*pi so the approximation in
   * double is castable in float. */
  *approximatedAngle =
      static_cast<float>(Shared::PoincareHelpers::ApproximateToScalar<double>(
          approximateAngle.isUninitialized() ? exactAngle : approximateAngle,
          ctx->m_context,
          {.complexFormat = ctx->m_complexFormat,
           .angleUnit = ctx->m_angleUnit}));
  *approximatedAngle = NewTrigonometry::ConvertAngleToRadian(*approximatedAngle,
                                                             ctx->m_angleUnit);
}

UserExpression AdditionalResultHelper::ExtractExactAngleFromDirectTrigo(
    const UserExpression input, const UserExpression exactOutput,
    Context* context,
    const Preferences::CalculationPreferences calculationPreferences) {
  assert(!input.hasUnit(true));
  assert(!exactOutput.hasUnit(true));
  /* Trigonometry additional results are displayed if either input or output is
   * a direct function. Indeed, we want to capture both cases:
   * - > input: cos(60)
   *   > output: 1/2
   * - > input: 2cos(2) - cos(2)
   *   > output: cos(2)
   * However if the result is complex, it is treated as a complex result.
   * When both inputs and outputs are direct trigo functions, we take the input
   * because the angle might not be the same modulo 2π. */
  assert(!exactOutput.isScalarComplex(calculationPreferences));
  UserExpression directTrigoFunction;
  if (Trigonometry::IsDirectTrigonometryFunction(input) &&
      !input.deepIsSymbolic(context,
                            SymbolicComputation::DoNotReplaceAnySymbol)) {
    /* Do not display trigonometric additional informations, in case the symbol
     * value is later modified/deleted in the storage and can't be retrieved.
     * Ex: 0->x; tan(x); 3->x; => The additional results of tan(x) become
     * inconsistent. And if x is deleted, it crashes. */
    directTrigoFunction = input;
  } else if (Trigonometry::IsDirectTrigonometryFunction(exactOutput)) {
    directTrigoFunction = exactOutput;
  } else {
    return UserExpression();
  }
  assert(!directTrigoFunction.isUninitialized() &&
         !directTrigoFunction.isUndefined());
  UserExpression exactAngle = directTrigoFunction.childAtIndex(0);
  assert(!exactAngle.isUninitialized() && !exactAngle.isUndefined());
  assert(!exactAngle.hasUnit(true));
  Preferences::ComplexFormat complexFormat =
      calculationPreferences.complexFormat;
  Preferences::AngleUnit angleUnit = calculationPreferences.angleUnit;
  UserExpression unit;
  Shared::PoincareHelpers::CloneAndReduceAndRemoveUnit(
      &exactAngle, &unit, context,
      {.complexFormat = complexFormat, .angleUnit = angleUnit});
  if (!unit.isUninitialized()) {
    assert(Unit::IsPureAngleUnit(unit, true));
    /* After a reduction, all angle units are converted to radians, so we
     * convert exactAngle again here to fit the angle unit that will be used
     * in reductions below. */
    exactAngle = Multiplication::Builder(
        exactAngle, Trigonometry::UnitConversionFactor(
                        Preferences::AngleUnit::Radian, angleUnit));
  }
  // The angle must be real.
  if (!std::isfinite(Shared::PoincareHelpers::ApproximateToScalar<double>(
          exactAngle, context,
          {.complexFormat = complexFormat, .angleUnit = angleUnit}))) {
    return UserExpression();
  }
  assert(!exactAngle.isUninitialized() && !exactAngle.isUndefined());
  return exactAngle;
}

}  // namespace Poincare
