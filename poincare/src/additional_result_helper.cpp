#include <apps/shared/expression_display_permissions.h>
#include <apps/shared/poincare_helpers.h>
#include <poincare/additional_result_helper.h>
#include <poincare/k_tree.h>
#include <poincare/new_trigonometry.h>
#include <poincare/old/arc_cosine.h>
#include <poincare/old/cosine.h>
#include <poincare/old/sine.h>
#include <poincare/old/subtraction.h>
#include <poincare/src/expression/dimension.h>
#include <poincare/src/expression/k_tree.h>
#include <poincare/src/expression/projection.h>
#include <poincare/src/expression/simplification.h>
#include <poincare/src/expression/unit.h>
#include <poincare/src/memory/pattern_matching.h>

namespace Poincare {

using namespace Internal;

void AdditionalResultHelper::TrigonometryAngleHelper(
    const UserExpression input, const UserExpression exactOutput,
    const UserExpression approximateOutput, bool directTrigonometry,
    Poincare::Preferences::CalculationPreferences calculationPreferences,
    const ProjectionContext* ctx, UserExpression& exactAngle,
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
  const Tree* inputTree = input.tree();
  const Tree* exactTree = exactOutput.tree();
  assert(Internal::Dimension::Get(exactTree).isScalar() ||
         Internal::Dimension::Get(exactTree).isSimpleAngleUnit());
  /* Trigonometry additional results are displayed if either input or output is
   * a direct function. Indeed, we want to capture both cases:
   * - > input: cos(60)
   *   > output: 1/2
   * - > input: 2cos(2) - cos(2)
   *   > output: cos(2)
   * However if the result is complex, it is treated as a complex result.
   * When both inputs and outputs are direct trigo functions, we take the input
   * because the angle might not be the same modulo 2π. */
  const Tree* directTrigoFunction;
  if (inputTree->isDirectTrigonometryFunction() &&
      !inputTree->hasChildSatisfying(
          [](const Tree* e) { return e->isUserNamed(); })) {
    /* Do not display trigonometric additional informations, in case the symbol
     * value is later modified/deleted in the storage and can't be retrieved.
     * Ex: 0->x; tan(x); 3->x; => The additional results of tan(x) become
     * inconsistent. And if x is deleted, it crashes. */
    directTrigoFunction = inputTree;
  } else if (exactTree->isDirectTrigonometryFunction()) {
    directTrigoFunction = exactTree;
  } else {
    return UserExpression();
  }
  assert(directTrigoFunction && !directTrigoFunction->isUndefined());

  Tree* exactAngle = directTrigoFunction->child(0)->cloneTree();
  assert(exactAngle && !exactAngle->isUndefined());
  Internal::Dimension exactAngleDimension =
      Internal::Dimension::Get(exactAngle);
  assert(exactAngleDimension.isScalar() ||
         exactAngleDimension.isSimpleAngleUnit());
  Preferences::ComplexFormat complexFormat =
      calculationPreferences.complexFormat;
  AngleUnit angleUnit = calculationPreferences.angleUnit;
  ProjectionContext projCtx = {
      .m_complexFormat = complexFormat,
      .m_angleUnit = angleUnit,
      .m_symbolic =
          SymbolicComputation::ReplaceAllSymbolsWithDefinitionsOrUndefined,
      .m_context = context,
  };

  /* TODO: A simplification could be avoided by calling
   * SimplifyWithAdaptiveStrategy steps, and handle units right after
   * projection. */
  Simplification::SimplifyWithAdaptiveStrategy(exactAngle, &projCtx);
  if (exactAngleDimension.isSimpleAngleUnit()) {
    assert(directTrigoFunction->isDirectTrigonometryFunction());
    /* When removing units, angle units are converted to radians, so we
     * manually add the conversion ratio back to preserve the input angleUnit.
     */
    // exactAngle * angleUnitRatio / RadianUnitRatio
    exactAngle->cloneNodeAtNode(KMult.node<3>);
    Units::Unit::Push(angleUnit);
    KPow->cloneNode();
    Units::Unit::Push(AngleUnit::Radian);
    (-1_e)->cloneTree();
    // Remove units
    Tree::ApplyShallowInDepth(exactAngle, Units::Unit::ShallowRemoveUnit);
    // Simplify again
    Simplification::SimplifyWithAdaptiveStrategy(exactAngle, &projCtx);
  }

  // The angle must be real and finite.
  if (!std::isfinite(Approximation::RootTreeToReal<float>(exactAngle, angleUnit,
                                                          complexFormat))) {
    exactAngle->removeTree();
    return UserExpression();
  }
  return UserExpression::Builder(exactAngle);
}

}  // namespace Poincare
