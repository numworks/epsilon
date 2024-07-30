#include <poincare/additional_results_helper.h>
#include <poincare/new_trigonometry.h>
#include <poincare/src/expression/angle.h>
#include <poincare/src/expression/dimension.h>
#include <poincare/src/expression/k_tree.h>
#include <poincare/src/expression/projection.h>
#include <poincare/src/expression/simplification.h>
#include <poincare/src/expression/unit.h>
#include <poincare/src/expression/unit_representatives.h>
#include <poincare/src/memory/pattern_matching.h>

namespace Poincare {

using namespace Internal;

void AdditionalResultsHelper::TrigonometryAngleHelper(
    const UserExpression input, const UserExpression exactOutput,
    const UserExpression approximateOutput, bool directTrigonometry,
    Poincare::Preferences::CalculationPreferences calculationPreferences,
    const ProjectionContext* ctx,
    ShouldOnlyDisplayApproximation shouldOnlyDisplayApproximation,
    UserExpression& exactAngle, float* approximatedAngle, bool* angleIsExact) {
  assert(approximatedAngle && angleIsExact);
  const Tree* period = Angle::Period(ctx->m_angleUnit);
  TreeRef approximateAngleTree;
  // Find the angle
  if (directTrigonometry) {
    exactAngle = ExtractExactAngleFromDirectTrigo(
        input, exactOutput, ctx->m_context, calculationPreferences);
  } else {
    exactAngle = exactOutput;
    approximateAngleTree = approximateOutput.tree()->cloneTree();
    assert(approximateAngleTree && !approximateAngleTree->isUndefined());
    if (Sign::Get(approximateAngleTree).isStrictlyNegative()) {
      // If the approximate angle is in [-π, π], set it in [0, 2π]
      approximateAngleTree = PatternMatching::Create(
          KAdd(KA, KB), {.KA = period, .KB = approximateAngleTree});
    }
  }
  assert(!exactAngle.isUninitialized() && !exactAngle.isUndefined());

  /* Set exact angle in [0, 2π].
   * Use the reduction of frac part to compute modulo. */
  TreeRef simplifiedAngle = PatternMatching::Create(
      KMult(KFrac(KDiv(KA, KB)), KB), {.KA = exactAngle, .KB = period});
  ProjectionContext contextCopy = *ctx;
  Simplification::SimplifyWithAdaptiveStrategy(simplifiedAngle, &contextCopy);
  // Reduction is not expected to have failed
  assert(!simplifiedAngle.isUninitialized());

  /* Approximate the angle if:
   * - The fractional part could not be reduced (because the angle is not a
   * multiple of pi)
   * - Displaying the exact expression is forbidden. */
  if (simplifiedAngle->hasDescendantSatisfying(
          [](const Tree* e) { return e->isFrac(); }) ||
      shouldOnlyDisplayApproximation(
          exactAngle,
          UserExpression::Builder(static_cast<const Tree*>(simplifiedAngle)),
          UserExpression::Builder(
              static_cast<const Tree*>(approximateAngleTree)),
          ctx->m_context)) {
    if (directTrigonometry) {
      assert(approximateAngleTree.isUninitialized());
      /* Do not approximate the FracPart, which could lead to truncation error
       * for large angles (e.g. frac(1e17/2pi) = 0). Instead find the angle with
       * the same sine and cosine. */
      approximateAngleTree =
          PatternMatching::Create(KACos(KCos(KA)), {.KA = exactAngle});
      /* acos has its values in [0,π[, use the sign of the sine to find the
       * right semicircle. */
      Tree* sine = PatternMatching::Create(KSin(KA), {.KA = exactAngle});
      bool removePeriod = Approximation::RootTreeToReal<double>(
                              sine, ctx->m_angleUnit, ctx->m_complexFormat) < 0;
      sine->removeTree();
      if (removePeriod) {
        approximateAngleTree = PatternMatching::Create(
            KSub(KA, KB), {.KA = period, .KB = approximateAngleTree});
      }
    }
    assert(!approximateAngleTree.isUninitialized());
    MoveTreeOverTree(
        approximateAngleTree,
        Approximation::RootTreeToTree<double>(
            approximateAngleTree, ctx->m_angleUnit, ctx->m_complexFormat));
    exactAngle =
        UserExpression::Builder(static_cast<const Tree*>(approximateAngleTree));
    *angleIsExact = false;
  } else {
    exactAngle =
        UserExpression::Builder(static_cast<const Tree*>(simplifiedAngle));
    *angleIsExact = true;
  }
  assert(!exactAngle.isUninitialized() && !exactAngle.isUndefined());

  /* m_model ask for a float angle but we compute the angle in double and then
   * cast it to float because approximation in float can overflow during the
   * computation. The angle should be between 0 and 2*pi so the approximation in
   * double is castable in float. */
  *approximatedAngle = static_cast<float>(Approximation::RootTreeToReal<double>(
      approximateAngleTree.isUninitialized() ? simplifiedAngle
                                             : approximateAngleTree,
      ctx->m_angleUnit, ctx->m_complexFormat));
  simplifiedAngle->removeTree();
  if (!approximateAngleTree.isUninitialized()) {
    approximateAngleTree->removeTree();
  }
  *approximatedAngle = NewTrigonometry::ConvertAngleToRadian(*approximatedAngle,
                                                             ctx->m_angleUnit);
}

/* Returns a (unreduced) division between pi in each unit, or 1 if the units
 * are the same. */
Tree* PushUnitConversionFactor(Preferences::AngleUnit fromUnit,
                               Preferences::AngleUnit toUnit) {
  if (fromUnit == toUnit) {
    // Just an optimisation to gain some time at reduction
    return (1_e)->cloneTree();
  }
  return PatternMatching::Create(
      KDiv(KA, KB),
      {.KA = Units::Angle::DefaultRepresentativeForAngleUnit(toUnit)
                 ->ratioExpression(),
       .KB = Units::Angle::DefaultRepresentativeForAngleUnit(fromUnit)
                 ->ratioExpression()});
}

UserExpression AdditionalResultsHelper::ExtractExactAngleFromDirectTrigo(
    const UserExpression input, const UserExpression exactOutput,
    Context* context,
    const Preferences::CalculationPreferences calculationPreferences) {
  const Tree* inputTree = input.tree();
  const Tree* exactTree = exactOutput.tree();
  assert(Internal::Dimension::Get(inputTree).isScalar() ||
         Internal::Dimension::Get(inputTree).isSimpleAngleUnit());
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
  assert(!exactOutput.isScalarComplex(calculationPreferences));
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

  /* TODO: Second SimplifyWithAdaptiveStrategy could be avoided by calling
   * intermediate steps, and handle units right after projection. */
  Simplification::SimplifyWithAdaptiveStrategy(exactAngle, &projCtx);
  if (exactAngleDimension.isUnit()) {
    assert(exactAngleDimension.isSimpleAngleUnit());
    assert(directTrigoFunction->isDirectTrigonometryFunction());
    /* When removing units, angle units are converted to radians, so we
     * manually add the conversion ratio back to preserve the input angleUnit.
     */
    // exactAngle * angleUnitRatio / RadianUnitRatio
    Tree::ApplyShallowInDepth(exactAngle, Units::Unit::ShallowRemoveUnit);
    exactAngle->cloneNodeAtNode(KMult.node<2>);
    PushUnitConversionFactor(AngleUnit::Radian, angleUnit);
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
