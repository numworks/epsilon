#include "additional_results_type.h"

#include <poincare/additional_results_helper.h>
#include <poincare/old/context.h>
#include <poincare/preferences.h>

#include <cmath>

#include "../calculation.h"
#include "vector_helper.h"

using namespace Poincare;
using namespace Shared;

namespace Calculation {

AdditionalResultsType AdditionalResultsType::AdditionalResultsForExpressions(
    const UserExpression input, const UserExpression exactOutput,
    const UserExpression approximateOutput,
    const Preferences::CalculationPreferences calculationPreferences,
    Poincare::Context* context) {
  if (ForbidAdditionalResults(input, exactOutput, approximateOutput)) {
    return AdditionalResultsType{.empty = true};
  }
  if (HasComplex(approximateOutput, calculationPreferences, context)) {
    return AdditionalResultsType{.complex = true};
  }
  if (HasComplex(exactOutput, calculationPreferences, context)) {
    // Cf comment in HasComplex
    return AdditionalResultsType{.empty = true};
  }
  bool inputHasAngleUnit, exactHasAngleUnit, approximateHasAngleUnit;
  bool inputHasUnit = input.hasUnit(true, &inputHasAngleUnit);
  bool exactHasUnit = exactOutput.hasUnit(true, &exactHasAngleUnit);
  bool approximateHasUnit =
      approximateOutput.hasUnit(true, &approximateHasAngleUnit);
  assert(exactHasUnit == approximateHasUnit);
  (void)approximateHasUnit;  // Silence compiler
  if (inputHasUnit || exactHasUnit) {
    /* We display units additional results based on exact output. If input has
     * units but not output (ex: L/(L/3)), we don't display any results. */
    return exactHasUnit && HasUnit(exactOutput, calculationPreferences)
               ? AdditionalResultsType{.unit = true}
               : AdditionalResultsType{.empty = true};
  }
  if (HasDirectTrigo(input, exactOutput, calculationPreferences, context)) {
    return AdditionalResultsType{.directTrigonometry = true};
  }
  if (HasInverseTrigo(input, exactOutput, calculationPreferences, context)) {
    return AdditionalResultsType{.inverseTrigonometry = true};
  }
  if (HasVector(exactOutput, approximateOutput, calculationPreferences,
                context)) {
    return AdditionalResultsType{.vector = true};
  }
  if (approximateOutput.dimension().isMatrix()) {
    return HasMatrix(approximateOutput) ? AdditionalResultsType{.matrix = true}
                                        : AdditionalResultsType{.empty = true};
  }
  if (exactHasAngleUnit || approximateHasAngleUnit) {
    return exactHasAngleUnit && HasUnit(exactOutput, calculationPreferences)
               ? AdditionalResultsType{.unit = true}
               : AdditionalResultsType{.empty = true};
  }
  AdditionalResultsType type = {};
  if (!inputHasAngleUnit && HasFunction(input, approximateOutput)) {
    type.function = true;
  }
  if (HasScientificNotation(approximateOutput, calculationPreferences,
                            context)) {
    type.scientificNotation = true;
  }
  if (HasInteger(exactOutput)) {
    type.integer = true;
  } else if (HasRational(exactOutput)) {
    type.rational = true;
  }
  if (type.isUninitialized()) {
    type.empty = true;
  }
  assert(!type.isUninitialized());
  return type;
}

bool AdditionalResultsType::ForbidAdditionalResults(
    const UserExpression input, const UserExpression exactOutput,
    const UserExpression approximateOutput) {
  /* Special case for Store:
   * Store nodes have to be at the root of the expression, which prevents
   * from creating new expressions with store node as a child. We don't
   * return any additional outputs for them to avoid bothering with special
   * cases. */
  if (Preferences::SharedPreferences()->examMode().forbidAdditionalResults() ||
      input.isUninitialized() || exactOutput.isUninitialized() ||
      approximateOutput.isUninitialized() || input.isStore() ||
      exactOutput.isList() || approximateOutput.isList() ||
      approximateOutput.recursivelyMatches([](const NewExpression e) {
        return e.isUndefinedOrNonReal() || e.isPlusOrMinusInfinity();
      })) {
    return true;
  }
  assert(!input.isUndefined() && !exactOutput.isUndefined());
  return false;
}

bool AdditionalResultsType::HasComplex(
    const UserExpression approximateOutput,
    const Preferences::CalculationPreferences calculationPreferences,
    Context* context) {
  /* We have 2 edge cases:
   * 1) exact output assessed to scalar complex but not approximate output
   * ex:
   *    In polar format, for input -10 the exact output is 10e^(iπ) and the
   *    approximate output is 10e^(3.14i). Due to rounding errors, the imaginary
   *    part of the approximate output is not zero so it is considered as scalar
   *    complex, while the exact output is not.
   * 2) approximate output assessed to scalar complex but not exact output
   * ex:
   *    For input i^(2×e^(7i^(2×e^322))), the exact output approximates to a
   *    complex with very small norm but PrintFloat::ConvertFloatToTextPrivate
   *    rounds it to 0 so the approximation output is 0i. Thus, the imaginary
   *    part of the approximate output is zero so it is not considered as scalar
   *    complex, while the exact output is.
   * We chosed to handle the 2nd case and not to display any additional results
   * in the 1st case. */
  return approximateOutput.isScalarComplex(calculationPreferences, context);
}

bool AdditionalResultsType::HasDirectTrigo(
    const UserExpression input, const UserExpression exactOutput,
    const Preferences::CalculationPreferences calculationPreferences,
    Context* context) {
  assert(!exactOutput.hasUnit(true));
  Expression exactAngle =
      AdditionalResultsHelper::ExtractExactAngleFromDirectTrigo(
          input, exactOutput, context, calculationPreferences);
  return !exactAngle.isUninitialized();
}

bool AdditionalResultsType::HasInverseTrigo(
    const UserExpression input, const UserExpression exactOutput,
    const Preferences::CalculationPreferences calculationPreferences,
    Poincare::Context* context) {
  // If the result is complex, it is treated as a complex result instead.
  assert(!HasComplex(exactOutput, calculationPreferences, context));
  assert(!exactOutput.hasUnit(true));
  return AdditionalResultsHelper::HasInverseTrigo(input, exactOutput);
}

bool AdditionalResultsType::HasUnit(
    const UserExpression exactOutput,
    const Preferences::CalculationPreferences calculationPreferences) {
  // HasUnit is only called when exactOutput has Units
  assert(exactOutput.hasUnit());
#if 1  // TODO_PCJ
  // Assume units that cancel themselves have been removed by simplification.
  double value = exactOutput.approximateToRealScalar<double>(
      calculationPreferences.angleUnit, calculationPreferences.complexFormat);
  /* TODO_PCJ: For now we assume there will always be AdditionalOutputs to
   * display if approximation is finite. We should simplify the exact output
   * with each relevant UnitDisplay and return false if all of them produce the
   * same as exactOutput. */
  return std::isfinite(value) && exactOutput.dimension().isUnit();
#else
  assert(exactOutput.dimension().isUnit());
  Context* globalContext =
      AppsContainerHelper::sharedAppsContainerGlobalContext();
  Preferences::ComplexFormat complexFormat =
      calculationPreferences.complexFormat;
  Preferences::AngleUnit angleUnit = calculationPreferences.angleUnit;
  UserExpression unit;
  UserExpression clone = exactOutput.clone();
  PoincareHelpers::CloneAndReduceAndRemoveUnit(
      &clone, &unit, globalContext,
      {.complexFormat = complexFormat,
       .angleUnit = angleUnit,
       .symbolicComputation = SymbolicComputation::ReplaceAllSymbols,
       .unitConversion = UnitConversion::None});
  double value =
      clone.approximateToRealScalar<double>(angleUnit, complexFormat);
  if (!unit.isUninitialized() &&
      (Unit::ShouldDisplayAdditionalOutputs(
           value, unit,
           GlobalPreferences::SharedGlobalPreferences()->unitFormat()) ||
       UnitComparison::ShouldDisplayUnitComparison(value, unit))) {
    /* Sometimes with angle units, the reduction with UnitConversion::None
     * will be defined but not the reduction with UnitConversion::Default,
     * which will make the unit list controller crash.  */
    unit = UserExpression();
    clone = exactOutput.clone();
    PoincareHelpers::CloneAndReduceAndRemoveUnit(
        &clone, &unit, globalContext,
        {.complexFormat = complexFormat, .angleUnit = angleUnit});
    return !unit.isUninitialized();
  }
  return false;
#endif
}

bool AdditionalResultsType::HasVector(
    const UserExpression exactOutput, const UserExpression approximateOutput,
    const Preferences::CalculationPreferences calculationPreferences,
    Context* context) {
  Expression norm = VectorHelper::BuildVectorNorm(exactOutput.clone(), context,
                                                  calculationPreferences);
  if (norm.isUninitialized()) {
    return false;
  }
  assert(!norm.isUndefined());
  int nChildren = approximateOutput.tree()->numberOfChildren();
  for (int i = 0; i < nChildren; ++i) {
    if (HasComplex(approximateOutput, calculationPreferences, context)) {
      return false;
    }
  }
  return true;
}

bool AdditionalResultsType::HasMatrix(const UserExpression approximateOutput) {
  assert(!approximateOutput.isUninitialized());
  assert(!approximateOutput.hasUnit(true));
  return approximateOutput.isMatrix() &&
         !approximateOutput.recursivelyMatches(&NewExpression::isUndefined);
}

bool AdditionalResultsType::HasFunction(
    const UserExpression input, const UserExpression approximateOutput) {
  // We want a single numerical value and to avoid showing the identity function
  assert(!input.isUninitialized());
  assert(!input.hasUnit());
  assert(!approximateOutput.isUndefined());
  assert(!approximateOutput.hasUnit());
  assert(!approximateOutput.isMatrix());
  return !approximateOutput.isNonReal() && !approximateOutput.isPoint() &&
         AdditionalResultsHelper::expressionIsInterestingFunction(input);
}

bool AdditionalResultsType::HasScientificNotation(
    const UserExpression approximateOutput,
    const Preferences::CalculationPreferences calculationPreferences,
    Context* context) {
  assert(!approximateOutput.isUninitialized());
  assert(!approximateOutput.hasUnit());
  if (approximateOutput.isNonReal() ||
      calculationPreferences.displayMode ==
          Preferences::PrintFloatMode::Scientific) {
    return false;
  }
  Poincare::Layout historyResult = approximateOutput.createLayout(
      calculationPreferences.displayMode,
      calculationPreferences.numberOfSignificantDigits, context);
  return !historyResult.isIdenticalTo(
      AdditionalResultsHelper::ScientificLayout(approximateOutput, context,
                                                calculationPreferences),
      true);
}

bool AdditionalResultsType::HasInteger(
    const Poincare::UserExpression exactOutput) {
  assert(!exactOutput.hasUnit());
  return Poincare::AdditionalResultsHelper::HasInteger(exactOutput);
}

bool AdditionalResultsType::HasRational(
    const Poincare::UserExpression exactOutput) {
  assert(!exactOutput.hasUnit());
  return Poincare::AdditionalResultsHelper::HasRational(exactOutput);
}

}  // namespace Calculation
