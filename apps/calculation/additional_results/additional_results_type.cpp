#include "additional_results_type.h"

#include <apps/apps_container_helper.h>
#include <poincare/additional_results_helper.h>
#include <poincare/old/trigonometry.h>
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
    const Preferences::CalculationPreferences calculationPreferences) {
  if (ForbidAdditionalResults(input, exactOutput, approximateOutput)) {
    return AdditionalResultsType{.empty = true};
  }
  if (HasComplex(approximateOutput, calculationPreferences)) {
    return AdditionalResultsType{.complex = true};
  }
  if (exactOutput.isScalarComplex(calculationPreferences)) {
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
  if (HasDirectTrigo(input, exactOutput, calculationPreferences)) {
    return AdditionalResultsType{.directTrigonometry = true};
  }
  if (HasInverseTrigo(input, exactOutput, calculationPreferences)) {
    return AdditionalResultsType{.inverseTrigonometry = true};
  }
  if (HasVector(exactOutput, approximateOutput, calculationPreferences)) {
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
  if (HasScientificNotation(approximateOutput, calculationPreferences)) {
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
      NewExpression::IsUninitialized(input) ||
      NewExpression::IsUninitialized(exactOutput) ||
      NewExpression::IsUninitialized(approximateOutput) ||
      NewExpression::IsStore(input) || NewExpression::IsList(exactOutput) ||
      NewExpression::IsList(approximateOutput) ||
      approximateOutput.recursivelyMatches([](const NewExpression e) {
        return NewExpression::IsUndefined(e) ||
               NewExpression::IsPlusOrMinusInfinity(e);
      })) {
    return true;
  }
  assert(!NewExpression::IsUndefined(input) &&
         !NewExpression::IsUndefined(exactOutput));
  return false;
}

bool AdditionalResultsType::HasComplex(
    const UserExpression approximateOutput,
    const Preferences::CalculationPreferences calculationPreferences) {
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
  return approximateOutput.isScalarComplex(calculationPreferences);
}

bool AdditionalResultsType::HasDirectTrigo(
    const UserExpression input, const UserExpression exactOutput,
    const Preferences::CalculationPreferences calculationPreferences) {
  assert(!exactOutput.hasUnit(true));
  Context* globalContext =
      AppsContainerHelper::sharedAppsContainerGlobalContext();
  Expression exactAngle =
      AdditionalResultsHelper::ExtractExactAngleFromDirectTrigo(
          input, exactOutput, globalContext, calculationPreferences);
  return !exactAngle.isUninitialized();
}

bool AdditionalResultsType::HasInverseTrigo(
    const UserExpression input, const UserExpression exactOutput,
    const Preferences::CalculationPreferences calculationPreferences) {
  // If the result is complex, it is treated as a complex result instead.
  assert(!exactOutput.isScalarComplex(calculationPreferences));
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
  double value = exactOutput.approximateUserExpressionToScalar<double>(
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
       .symbolicComputation =
           SymbolicComputation::ReplaceAllSymbolsWithDefinitionsOrUndefined,
       .unitConversion = UnitConversion::None});
  double value = PoincareHelpers::ApproximateToScalar<double>(
      clone, globalContext,
      {.complexFormat = complexFormat, .angleUnit = angleUnit});
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
    const Preferences::CalculationPreferences calculationPreferences) {
  Context* globalContext =
      AppsContainerHelper::sharedAppsContainerGlobalContext();
  Expression norm = VectorHelper::BuildVectorNorm(
      exactOutput.clone(), globalContext, calculationPreferences);
  if (norm.isUninitialized()) {
    return false;
  }
  assert(!norm.isUndefined());
  int nChildren = approximateOutput.tree()->numberOfChildren();
  for (int i = 0; i < nChildren; ++i) {
    if (approximateOutput.cloneChildAtIndex(i).isScalarComplex(
            calculationPreferences)) {
      return false;
    }
  }
  return true;
}

bool AdditionalResultsType::HasMatrix(const UserExpression approximateOutput) {
  assert(!approximateOutput.isUninitialized());
  assert(!approximateOutput.hasUnit(true));
  return NewExpression::IsMatrix(approximateOutput) &&
         !approximateOutput.recursivelyMatches(NewExpression::IsUndefined);
}

bool AdditionalResultsType::HasFunction(
    const UserExpression input, const UserExpression approximateOutput) {
  // We want a single numerical value and to avoid showing the identity function
  assert(!input.isUninitialized());
  assert(!input.hasUnit());
  assert(!approximateOutput.isUndefined());
  assert(!approximateOutput.hasUnit());
  assert(!NewExpression::IsMatrix(approximateOutput));
  return !NewExpression::IsNonReal(approximateOutput) &&
         !NewExpression::IsPoint(approximateOutput) &&
         AdditionalResultsHelper::expressionIsInterestingFunction(input);
}

bool AdditionalResultsType::HasScientificNotation(
    const UserExpression approximateOutput,
    const Preferences::CalculationPreferences calculationPreferences) {
  assert(!approximateOutput.isUninitialized());
  assert(!approximateOutput.hasUnit());
  Context* globalContext =
      AppsContainerHelper::sharedAppsContainerGlobalContext();
  if (NewExpression::IsNonReal(approximateOutput) ||
      calculationPreferences.displayMode ==
          Preferences::PrintFloatMode::Scientific) {
    return false;
  }
  Poincare::Layout historyResult = approximateOutput.createLayout(
      calculationPreferences.displayMode,
      calculationPreferences.numberOfSignificantDigits, globalContext);
  return !historyResult.isIdenticalTo(
      AdditionalResultsHelper::ScientificLayout(
          approximateOutput, globalContext, calculationPreferences),
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
