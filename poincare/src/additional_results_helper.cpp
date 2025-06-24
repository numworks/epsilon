#include <omg/ieee754.h>
#include <poincare/additional_results_helper.h>
#include <poincare/src/expression/angle.h>
#include <poincare/src/expression/beautification.h>
#include <poincare/src/expression/dimension.h>
#include <poincare/src/expression/float_helper.h>
#include <poincare/src/expression/integer.h>
#include <poincare/src/expression/k_tree.h>
#include <poincare/src/expression/matrix.h>
#include <poincare/src/expression/projection.h>
#include <poincare/src/expression/rational.h>
#include <poincare/src/expression/sign.h>
#include <poincare/src/expression/simplification.h>
#include <poincare/src/expression/systematic_reduction.h>
#include <poincare/src/expression/units/representatives.h>
#include <poincare/src/expression/units/unit.h>
#include <poincare/src/layout/layouter.h>
#include <poincare/src/memory/pattern_matching.h>
#include <poincare/src/memory/tree.h>
#include <poincare/trigonometry.h>

namespace Poincare {

using namespace Internal;

AdditionalResultsHelper::TrigonometryResults
AdditionalResultsHelper::TrigonometryAngleHelper(
    const UserExpression input, const UserExpression exactOutput,
    const UserExpression approximateOutput, bool directTrigonometry,
    Poincare::Preferences::CalculationPreferences calculationPreferences,
    const ProjectionContext* ctx,
    ShouldOnlyDisplayApproximation shouldOnlyDisplayApproximation) {
  const Tree* period = Angle::Period(ctx->m_angleUnit);
  // Find the angle
  UserExpression exactAngle =
      directTrigonometry
          ? ExtractExactAngleFromDirectTrigo(input, exactOutput, ctx->m_context,
                                             calculationPreferences)
          : exactOutput;
  assert(!exactAngle.isUninitialized() && !exactAngle.isUndefined());

  /* Set exact angle in [0, 2π[.
   * Use the reduction of frac part to compute modulo. */
  Tree* simplifiedAngle = PatternMatching::Create(
      KMult(KFrac(KDiv(KA, KB)), KB), {.KA = exactAngle, .KB = period});
  bool reductionSuccess = Simplification::Simplify(simplifiedAngle, *ctx);
#if ASSERTIONS
  {
    // Check that simplifiedAngle was set inside [0, 2π[
    double approximatedAngle = Approximation::To<double>(
        simplifiedAngle,
        Approximation::Parameters{.projectLocalVariables = true},
        Approximation::Context(ctx->m_angleUnit, ctx->m_complexFormat,
                               ctx->m_context));
    approximatedAngle =
        Trigonometry::ConvertAngleToRadian(approximatedAngle, ctx->m_angleUnit);
    assert(0.0 <= approximatedAngle && approximatedAngle <= 2.0 * M_PI);
  }
#endif

  Tree* approximateAngleTree = nullptr;
  if (!directTrigonometry) {
    approximateAngleTree = approximateOutput.tree()->cloneTree();
    assert(approximateAngleTree && !approximateAngleTree->isUndefined());
    if (GetSign(approximateAngleTree).isStrictlyNegative()) {
      // If the approximate angle is in [-π, π], set it in [0, 2π]
      approximateAngleTree->moveTreeOverTree(PatternMatching::Create(
          KAdd(KA, KB), {.KA = period, .KB = approximateAngleTree}));
    }
  }

  /* Approximate the angle if:
   * - Reduction was unsuccessful
   * - The fractional part could not be reduced
   * - Displaying the exact expression is forbidden. */
  bool angleIsExact = true;
  if (!reductionSuccess ||
      simplifiedAngle->hasDescendantSatisfying([](const Tree* e) {
        return e->isFrac() || e->isCeil() || e->isFloor();
      }) ||
      shouldOnlyDisplayApproximation(
          exactAngle,
          UserExpression::Builder(static_cast<const Tree*>(simplifiedAngle)),
          UserExpression::Builder(
              static_cast<const Tree*>(approximateAngleTree)),
          ctx->m_context)) {
    if (directTrigonometry) {
      assert(!approximateAngleTree);
      /* Do not approximate the FracPart, which could lead to truncation error
       * for large angles (e.g. frac(1e17/2pi) = 0). Instead find the angle with
       * the same sine and cosine. */
      approximateAngleTree =
          PatternMatching::Create(KACos(KCos(KA)), {.KA = exactAngle});
      /* acos has its values in [0,π[, use the sign of the sine to find the
       * right semicircle. */
      Tree* sine = PatternMatching::Create(KSin(KA), {.KA = exactAngle});
      bool removePeriod =
          Approximation::To<double>(
              sine, Approximation::Parameters{.projectLocalVariables = true},
              Approximation::Context(ctx->m_angleUnit, ctx->m_complexFormat,
                                     ctx->m_context)) < 0;
      sine->removeTree();
      if (removePeriod) {
        approximateAngleTree->moveTreeOverTree(PatternMatching::Create(
            KSub(KA, KB), {.KA = period, .KB = approximateAngleTree}));
      }
    }
    assert(approximateAngleTree);
    approximateAngleTree->moveTreeOverTree(Approximation::ToTree<double>(
        approximateAngleTree,
        Approximation::Parameters{.projectLocalVariables = true},
        Approximation::Context(ctx->m_angleUnit, ctx->m_complexFormat,
                               ctx->m_context)));
    Beautification::DeepBeautify(approximateAngleTree, *ctx);
    exactAngle =
        UserExpression::Builder(static_cast<const Tree*>(approximateAngleTree));
    angleIsExact = false;
  } else {
    exactAngle =
        UserExpression::Builder(static_cast<const Tree*>(simplifiedAngle));
  }
  assert(!exactAngle.isUninitialized() && !exactAngle.isUndefined());

  /* m_model ask for a float angle but we compute the angle in double and then
   * cast it to float because approximation in float can overflow during the
   * computation. The angle should be between 0 and 2*pi so the approximation in
   * double is castable in float. */
  assert(approximateAngleTree ||
         simplifiedAngle->treeIsIdenticalTo(exactAngle.tree()));
  double approximatedAngle = Approximation::To<double>(
      approximateAngleTree ? approximateAngleTree : simplifiedAngle,
      Approximation::Parameters{.projectLocalVariables = true},
      Approximation::Context(ctx->m_angleUnit, ctx->m_complexFormat,
                             ctx->m_context));
  if (approximateAngleTree) {
    approximateAngleTree->removeTree();
  }
  simplifiedAngle->removeTree();
  approximatedAngle =
      Trigonometry::ConvertAngleToRadian(approximatedAngle, ctx->m_angleUnit);
  assert(0.0 <= approximatedAngle && approximatedAngle <= 2.0 * M_PI);
  return {.exactAngle = exactAngle,
          .approximatedAngle = approximatedAngle,
          .angleIsExact = angleIsExact};
}

UserExpression AdditionalResultsHelper::ExtractExactAngleFromDirectTrigo(
    const UserExpression input, const UserExpression exactOutput,
    Context* context,
    const Preferences::CalculationPreferences calculationPreferences) {
  const Tree* inputTree = input.tree();
  const Tree* exactTree = exactOutput.tree();
  Internal::Dimension dimension = Internal::Dimension::Get(inputTree, context);

  assert(dimension == Internal::Dimension::Get(exactTree, context));
  if (!dimension.isScalarOrUnit() ||
      Internal::Dimension::IsList(exactTree, context) ||
      (dimension.isUnit() && !dimension.isSimpleAngleUnit())) {
    return UserExpression();
  }
  assert(!dimension.isUnit() || dimension.isSimpleAngleUnit());
  /* Trigonometry additional results are displayed if either input or output is
   * a direct function. Indeed, we want to capture both cases:
   * - > input: cos(60)
   *   > output: 1/2
   * - > input: 2cos(2) - cos(2)
   *   > output: cos(2)
   * However if the result is complex, it is treated as a complex result.
   * When both inputs and outputs are direct trigo functions, we take the input
   * because the angle might not be the same modulo 2π. */
  assert(!exactOutput.isComplexScalar(calculationPreferences, context));
  const Tree* directTrigoFunction;
  if ((inputTree->isDirectTrigonometryFunction() ||
       inputTree->isDirectAdvancedTrigonometryFunction()) &&
      !inputTree->hasDescendantSatisfying(
          [](const Tree* e) { return e->isUserNamed(); })) {
    /* Do not display trigonometric additional informations, in case the symbol
     * value is later modified/deleted in the storage and can't be retrieved.
     * Ex: 0->x; tan(x); 3->x; => The additional results of tan(x) become
     * inconsistent. And if x is deleted, it crashes. */
    directTrigoFunction = inputTree;
  } else if (exactTree->isDirectTrigonometryFunction() ||
             exactTree->isDirectAdvancedTrigonometryFunction()) {
    directTrigoFunction = exactTree;
  } else {
    return UserExpression();
  }
  assert(directTrigoFunction && !directTrigoFunction->isUndefined());

  bool reductionFailure = false;
  Tree* exactAngle = directTrigoFunction->child(0)->cloneTree();
  assert(exactAngle && !exactAngle->isUndefined());
  Internal::Dimension exactAngleDimension =
      Internal::Dimension::Get(exactAngle, context);
  assert(exactAngleDimension.isScalar() ||
         exactAngleDimension.isSimpleAngleUnit());
  Preferences::ComplexFormat complexFormat =
      calculationPreferences.complexFormat;
  AngleUnit angleUnit = calculationPreferences.angleUnit;
  ProjectionContext projCtx = {
      .m_complexFormat = complexFormat,
      .m_angleUnit = angleUnit,
      .m_symbolic = SymbolicComputation::ReplaceAllSymbols,
      .m_context = context,
  };

  /* TODO: Second Simplify could be avoided by calling
   * intermediate steps, and handle units right after projection. */
  reductionFailure = !Simplification::Simplify(exactAngle, projCtx);
  if (!reductionFailure && exactAngleDimension.isUnit()) {
    assert(exactAngleDimension.isSimpleAngleUnit());
    assert(directTrigoFunction->isDirectTrigonometryFunction() ||
           directTrigoFunction->isDirectAdvancedTrigonometryFunction());
    /* When removing units, angle units are converted to radians, so we
     * manually add the conversion ratio back to preserve the input angleUnit.
     */
    // exactAngle / angleUnitRatio
    Tree::ApplyShallowTopDown(exactAngle, Units::Unit::ShallowRemoveUnit);
    exactAngle->cloneNodeAtNode(KMult.node<2>);
    Angle::RadTo(angleUnit)->cloneTree();
    // Simplify again
    SystematicReduction::ShallowReduce(exactAngle);
  }

  // The angle must be real and finite.
  if (reductionFailure ||
      !std::isfinite(Approximation::To<float>(
          exactAngle, Approximation::Parameters{.projectLocalVariables = true},
          Approximation::Context(angleUnit, complexFormat, context)))) {
    exactAngle->removeTree();
    return UserExpression();
  }
  return UserExpression::Builder(exactAngle);
}

bool AdditionalResultsHelper::expressionIsInterestingFunction(
    const UserExpression e) {
  assert(!e.isUninitialized());

  if (e.isOfType({Type::Opposite, Type::Parentheses})) {
    return AdditionalResultsHelper::expressionIsInterestingFunction(
        e.cloneChildAtIndex(0));
  }
  return !e.isConstantNumber() && !e.tree()->isUnitConversion() &&
         !e.deepIsOfType({Type::UserSequence, Type::Factor, Type::Re, Type::Im,
                          Type::Arg, Type::Conj}) &&
         AdditionalResultsHelper::HasSingleNumericalValue(e);
}

bool AdditionalResultsHelper::HasInverseTrigo(
    const UserExpression input, const UserExpression exactOutput,
    const UserExpression approximateOutput) {
  bool result = input.tree()->isInverseTrigonometryFunction() ||
                input.tree()->isInverseAdvancedTrigonometryFunction() ||
                exactOutput.tree()->isInverseAdvancedTrigonometryFunction() ||
                exactOutput.tree()->isInverseTrigonometryFunction();
  if (result) {
    /* Projection is needed here for an accurate sign prediction, to avoid
     * additional result on complex inverse trigo */
    Tree* output = approximateOutput.tree()->cloneTree();
    Internal::ProjectionContext ctx = Internal::ProjectionContext{
        .m_complexFormat = ComplexFormat::Cartesian};
    Internal::Simplification::ToSystem(output, &ctx);
    result = GetComplexSign(output).isReal();
    output->removeTree();
  }
  return result;
}

// Return the only numerical value found in e, nullptr if there are none or more
const Tree* getNumericalValueTree(const Tree* e, bool* error) {
  assert(!*error);
  assert(!e->isDep());
  // Escape if e has a random node, a user symbol or inf
  if (e->isRandomized() || e->isUserSymbol() || e->isInf()) {
    *error = true;
    return nullptr;
  }
  // e is not considered as a numerical value so that e^2 -> e^x
  if ((e->isNumber() && !e->isEulerE()) || e->isDecimal()) {
    if (!std::isfinite(
            Approximation::To<float>(e, Approximation::Parameters{}))) {
      *error = true;
      return nullptr;
    }
    assert(!*error);
    return e;
  }
  const Tree* result = nullptr;
  for (const Tree* child : e->children()) {
    const Tree* newResult = getNumericalValueTree(child, error);
    if (*error) {
      return nullptr;
    }
    if (newResult) {
      if (result) {
        if (e->isPow() || e->isPowReal()) {
          /* Ignore the exponent (newResult) if base (result) has a numerical
           * value so that 2^3 -> x^3 */
          assert(!*error);
          return result;
        }
        // Return nullptr if there are more than one numerical values
        *error = true;
        return nullptr;
      }
      result = newResult;
    }
  }
  assert(!*error);
  return result;
}

bool AdditionalResultsHelper::HasSingleNumericalValue(
    const UserExpression input) {
  bool dummy = false;
  return getNumericalValueTree(input.tree(), &dummy) != nullptr;
}

UserExpression AdditionalResultsHelper::CloneReplacingNumericalValuesWithSymbol(
    const UserExpression input, const char* symbol, float* value) {
  Tree* clone = input.tree()->cloneTree();
  bool dummy = false;
  Tree* numericalValue =
      const_cast<Tree*>(getNumericalValueTree(clone, &dummy));
  assert(numericalValue);
  *value =
      Approximation::To<float>(numericalValue, Approximation::Parameters{});
  numericalValue->moveTreeOverTree(SharedTreeStack->pushUserSymbol(symbol));
  return UserExpression::Builder(clone);
}

UserExpression AdditionalResultsHelper::EquivalentInteger(
    const Poincare::UserExpression exactOutput) {
  assert(HasPositiveInteger(exactOutput));
  if (exactOutput.tree()->isInteger()) {
    return exactOutput;
  }
  float value = FloatHelper::To(exactOutput.tree());
  assert(std::isfinite(value) && value == std::round(value));
  return UserExpression::Builder(Integer::Push(static_cast<int32_t>(value)));
}

bool AdditionalResultsHelper::HasPositiveInteger(
    const Poincare::UserExpression exactOutput) {
  if (exactOutput.tree()->isFloat()) {
    // Downcast to float to handle both double and float trees.
    float value = FloatHelper::To(exactOutput.tree());
    return std::isfinite(value) && value > 0 && value == std::round(value) &&
           value < OMG::IEEE754<float>::NonExactIntegerLimit() &&
           value < static_cast<float>(k_maxPositiveInteger);
  }
  return exactOutput.tree()->isPositiveInteger() &&
         Internal::IntegerHandler::Compare(
             Internal::Integer::Handler(exactOutput),
             Internal::Integer::Handler(
                 IntegerLiteral<k_maxPositiveInteger>{})) < 0;
}

bool AdditionalResultsHelper::HasRational(
    const Poincare::UserExpression exactOutput) {
  // Find forms like [12]/[23] or -[12]/[23]
  Internal::PatternMatching::Context ctx;
  // TODO: this should be isRational before the beautification
  return (Internal::PatternMatching::Match(exactOutput, KDiv(KA, KB), &ctx) ||
          Internal::PatternMatching::Match(exactOutput, KOpposite(KDiv(KA, KB)),
                                           &ctx)) &&
         ctx.getTree(KA)->isInteger() && ctx.getTree(KB)->isInteger();
}

IntegerHandler extractInteger(const Tree* e) {
  /* TODO_PCJ: is this usage of IntegerHandler correct ?
   * A quick experiment showed incorrect digits with large numbers ! */
  if (e->isOpposite()) {
    IntegerHandler i = extractInteger(e->child(0));
    i.setSign(InvertSign(i.sign()));
    return i;
  }
  assert(e->isInteger());
  return Integer::Handler(e);
}

SystemExpression AdditionalResultsHelper::CreateRational(const UserExpression e,
                                                         bool negative) {
  const Tree* eTree = e.tree();
  IntegerHandler numerator = extractInteger(eTree->child(0));
  if (negative) {
    numerator.setSign(InvertSign(numerator.sign()));
  }
  IntegerHandler denominator = extractInteger(eTree->child(1));
  return SystemExpression::Builder(Rational::Push(numerator, denominator));
}

// Take a rational a/b and create the euclidean division a=b*q+r
UserExpression AdditionalResultsHelper::CreateEuclideanDivision(
    SystemExpression rational) {
  assert(rational.isRational());
  IntegerHandler num = Rational::Numerator(rational);
  IntegerHandler den = Rational::Denominator(rational);
  DivisionResult<Tree*> division = IntegerHandler::Division(num, den);
  Tree* numTree = num.pushOnTreeStack();
  Tree* denTree = den.pushOnTreeStack();
  Tree* quotient = division.quotient->cloneTree();
  /* We beautify the quotient that may be negative and leave the rest unchanged
   * because we want to control the beautification order manually. */
  Beautification::DeepBeautify(quotient);
  assert(denTree->isPositiveInteger() &&
         division.remainder->isPositiveInteger());
  UserExpression result = UserExpression::Builder(PatternMatching::Create(
      KEqual(KA, KAdd(KMult(KB, KC), KD)), {.KA = numTree,
                                            .KB = denTree,
                                            .KC = quotient,
                                            .KD = division.remainder}));

  quotient->removeTree();
  denTree->removeTree();
  numTree->removeTree();
  division.remainder->removeTree();
  division.quotient->removeTree();

  return result;
}

UserExpression AdditionalResultsHelper::CreateMixedFraction(
    SystemExpression rational, bool mixedFractionsEnabled) {
  assert(rational.isRational());
  IntegerHandler num = Rational::Numerator(rational);
  IntegerHandler den = Rational::Denominator(rational);
  bool numIsNegative = num.strictSign() == StrictSign::Negative;
  num.setSign(NonStrictSign::Positive);
  // Push quotient and remainder
  DivisionResult<Tree*> division = IntegerHandler::Division(num, den);
  Tree* integerPart = division.quotient;
  // Push the fraction
  TreeRef fractionPart =
      Rational::Push(Integer::Handler(division.remainder), den);
  division.remainder->removeTree();
  // Beautify components separately
  Beautification::DeepBeautify(integerPart);
  Beautification::DeepBeautify(fractionPart);
  // If mixed fractions are enabled
  if (mixedFractionsEnabled) {
    integerPart->cloneNodeAtNode(KMixedFraction);
    if (numIsNegative) {
      integerPart->cloneNodeAtNode(KOpposite);
    }
    return UserExpression::Builder(integerPart);
  }
  // If mixed fractions don't exist in this country
  if (numIsNegative) {
    if (!integerPart->isZero()) {
      integerPart->cloneNodeAtNode(KOpposite);
    }
    integerPart->cloneNodeAtNode(KSub);
  } else {
    integerPart->cloneNodeAtNode(KAdd.node<2>);
  }
  return UserExpression::Builder(integerPart);
}

// Eat reduced expression's tree and return a beautified layout
Poincare::Layout CreateBeautifiedLayout(Tree* reducedExpression,
                                        ProjectionContext* ctx,
                                        Preferences::PrintFloatMode displayMode,
                                        uint8_t numberOfSignificantDigits) {
  Simplification::BeautifyReduced(reducedExpression, ctx);
  return Poincare::Layout::Builder(Layouter::LayoutExpression(
      reducedExpression, false, false, numberOfSignificantDigits, displayMode));
}

Poincare::Layout AdditionalResultsHelper::ScientificLayout(
    const UserExpression approximateOutput, Context* context,
    const Preferences::CalculationPreferences calculationPreferences) {
  assert(calculationPreferences.displayMode !=
         Preferences::PrintFloatMode::Scientific);
  ProjectionContext ctx = {.m_complexFormat = ComplexFormat::Cartesian,
                           .m_strategy = Strategy::ApproximateToFloat,
                           .m_symbolic = SymbolicComputation::ReplaceAllSymbols,
                           .m_context = context,
                           .m_advanceReduce = false};
  Tree* e = approximateOutput.tree()->cloneTree();
  Simplification::ProjectAndReduce(e, &ctx);
  assert(!ctx.m_dimension.isUnit());
  return CreateBeautifiedLayout(
      e, &ctx, Preferences::PrintFloatMode::Scientific,
      calculationPreferences.numberOfSignificantDigits);
}

void AdditionalResultsHelper::ComputeMatrixProperties(
    const UserExpression& exactOutput, const UserExpression& approximateOutput,
    Internal::ProjectionContext ctx, Preferences::PrintFloatMode displayMode,
    uint8_t numberOfSignificantDigits, Poincare::Layout& determinantL,
    Poincare::Layout& inverseL, Poincare::Layout& rowEchelonFormL,
    Poincare::Layout& reducedRowEchelonFormL, Poincare::Layout& traceL) {
  assert(approximateOutput.tree()->isMatrix());
  bool isApproximate = !exactOutput.tree()->isMatrix();
  Tree* matrix =
      (isApproximate ? approximateOutput : exactOutput).tree()->cloneTree();
  // The expression must be reduced to call matrix methods.
  Simplification::ProjectAndReduce(matrix, &ctx);
  bool isSquared = Internal::Matrix::NumberOfRows(matrix) ==
                   Internal::Matrix::NumberOfColumns(matrix);

  // 1. Matrix determinant if square matrix
  if (isSquared) {
    Tree* determinant = nullptr;
    Tree* matrixClone = matrix->cloneTree();
    if (!Internal::Matrix::RowCanonize(matrixClone, true, &determinant,
                                       isApproximate)) {
      determinant = (KUndefUnhandled)->cloneTree();
    }
    // TODO: Use ComplexSign or approximation to handle more complex cases
    bool determinantIsUndefinedOrNull =
        determinant->isUndefined() || determinant->isZero();
    // TODO_PCJ: Prevent having to update ctx here.
    Internal::Dimension previousDimension = ctx.m_dimension;
    ctx.m_dimension = Internal::Dimension::Get(determinant, ctx.m_context);
    determinantL = CreateBeautifiedLayout(determinant, &ctx, displayMode,
                                          numberOfSignificantDigits);
    ctx.m_dimension = previousDimension;
    matrixClone->removeTree();

    /* 2. Matrix inverse if invertible matrix
     * A squared matrix is invertible if and only if determinant is non null */
    if (!determinantIsUndefinedOrNull) {
      Tree* inverse = Internal::Matrix::Inverse(matrix, isApproximate);
      inverseL = CreateBeautifiedLayout(inverse, &ctx, displayMode,
                                        numberOfSignificantDigits);
    }
  }

  // 3. Matrix row echelon form
  Tree* reducedRowEchelonForm = matrix->cloneTree();
  if (Internal::Matrix::RowCanonize(reducedRowEchelonForm, false, nullptr,
                                    isApproximate)) {
    // Clone layouted tree to preserve reducedRowEchelonForm for next step.
    rowEchelonFormL =
        CreateBeautifiedLayout(reducedRowEchelonForm->cloneTree(), &ctx,
                               displayMode, numberOfSignificantDigits);

    /* 4. Matrix reduced row echelon form
     *    Computed from row echelon form to save computation time. */
    if (!Internal::Matrix::RowCanonize(reducedRowEchelonForm, true, nullptr,
                                       isApproximate)) {
      assert(false);
    }
    reducedRowEchelonFormL = CreateBeautifiedLayout(
        reducedRowEchelonForm, &ctx, displayMode, numberOfSignificantDigits);
  }

  // 5. Matrix trace if square matrix
  if (isSquared) {
    Tree* trace = Internal::Matrix::Trace(matrix);
    // TODO_PCJ: Prevent having to update ctx here.
    ctx.m_dimension = Internal::Dimension::Get(trace, ctx.m_context);
    traceL = CreateBeautifiedLayout(trace, &ctx, displayMode,
                                    numberOfSignificantDigits);
  }

  matrix->removeTree();
}

}  // namespace Poincare
