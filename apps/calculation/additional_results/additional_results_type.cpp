#include "additional_results_type.h"

#include <apps/apps_container_helper.h>
#include <poincare/preferences.h>
#include <poincare/trigonometry.h>
#include <poincare/unit.h>

#include "../calculation.h"
#include "scientific_notation_helper.h"
#include "trigonometry_helper.h"
#include "unit_comparison_helper.h"
#include "vector_helper.h"

using namespace Poincare;
using namespace Shared;

namespace Calculation {

AdditionalResultsType AdditionalResultsType::AdditionalResultsForExpressions(
    const Expression input, const Expression exactOutput,
    const Expression approximateOutput) {
  if (ForbidAdditionalResults(input, exactOutput, approximateOutput)) {
    return AdditionalResultsType{};
  }
  if (HasComplex(approximateOutput)) {
    return AdditionalResultsType{.complex = true};
  }
  if (HasDirectTrigo(input, exactOutput)) {
    return AdditionalResultsType{.directTrigonometry = true};
  }
  if (HasInverseTrigo(input, exactOutput)) {
    return AdditionalResultsType{.inverseTrigonometry = true};
  }
  if (exactOutput.hasUnit()) {
    return AdditionalResultsType{.unit = HasUnit(exactOutput)};
  }
  if (HasVector(exactOutput)) {
    return AdditionalResultsType{.vector = true};
  }
  if (exactOutput.deepIsMatrix()) {
    return AdditionalResultsType{.matrix = HasMatrix(exactOutput)};
  }
  AdditionalResultsType type = {};
  if (HasFunction(input, approximateOutput)) {
    type.function = true;
  }
  if (HasScientificNotation(approximateOutput)) {
    type.scientificNotation = true;
  }
  if (HasInteger(exactOutput)) {
    type.integer = true;
  } else if (HasRational(exactOutput)) {
    type.rational = true;
  }
  return type;
}

bool AdditionalResultsType::ForbidAdditionalResults(
    const Expression input, const Expression exactOutput,
    const Expression approximateOutput) {
  /* Special case for Store:
   * Store nodes have to be at the root of the expression, which prevents
   * from creating new expressions with store node as a child. We don't
   * return any additional outputs for them to avoid bothering with special
   * cases. */
  if (Preferences::sharedPreferences->examMode().forbidAdditionalResults() ||
      input.isUninitialized() || exactOutput.isUninitialized() ||
      approximateOutput.isUninitialized() || approximateOutput.isUndefined() ||
      input.type() == ExpressionNode::Type::Store ||
      exactOutput.type() == ExpressionNode::Type::List ||
      approximateOutput.type() == ExpressionNode::Type::List ||
      approximateOutput.recursivelyMatches(
          [](const Expression e) {
            return e.isOfType({ExpressionNode::Type::Infinity});
          },
          nullptr)) {
    return true;
  }
  assert(!input.isUndefined() && !exactOutput.isUndefined());
  return false;
}

bool AdditionalResultsType::HasComplex(const Expression approximateOutput) {
  /* Using the approximated output instead of the user input to guess the
   * complex format makes additional results more consistent when the user has
   * created complexes in Complex mode and then switched back to Real mode. */
  return approximateOutput.isScalarComplex(Preferences::sharedPreferences);
}

bool AdditionalResultsType::HasDirectTrigo(const Expression input,
                                           const Expression exactOutput) {
  Context *globalContext =
      AppsContainerHelper::sharedAppsContainerGlobalContext();
  Expression exactAngle = TrigonometryHelper::ExtractExactAngleFromDirectTrigo(
      input, exactOutput, globalContext);
  return !exactAngle.isUninitialized();
}

bool AdditionalResultsType::HasInverseTrigo(const Expression input,
                                            const Expression exactOutput) {
  // If the result is complex, it is treated as a complex result instead.
  assert(!exactOutput.isScalarComplex(Preferences::sharedPreferences));
  return (Trigonometry::isInverseTrigonometryFunction(input)) ||
         Trigonometry::isInverseTrigonometryFunction(exactOutput);
}

bool AdditionalResultsType::HasUnit(const Expression exactOutput) {
  assert(exactOutput.hasUnit());
  Context *globalContext =
      AppsContainerHelper::sharedAppsContainerGlobalContext();
  Expression unit;
  Expression clone = exactOutput.clone();
  PoincareHelpers::CloneAndReduceAndRemoveUnit(
      &clone, globalContext, ReductionTarget::User, &unit,
      SymbolicComputation::ReplaceAllSymbolsWithDefinitionsOrUndefined,
      UnitConversion::None);
  double value =
      PoincareHelpers::ApproximateToScalar<double>(clone, globalContext);
  if (!unit.isUninitialized() &&
      (Unit::ShouldDisplayAdditionalOutputs(
           value, unit,
           GlobalPreferences::sharedGlobalPreferences->unitFormat()) ||
       UnitComparison::ShouldDisplayUnitComparison(value, unit))) {
    /* Sometimes with angle units, the reduction with UnitConversion::None
     * will be defined but not the reduction with UnitConversion::Default,
     * which will make the unit list controller crash.  */
    unit = Expression();
    clone = exactOutput.clone();
    PoincareHelpers::CloneAndReduceAndRemoveUnit(&clone, globalContext,
                                                 ReductionTarget::User, &unit);
    return !unit.isUninitialized();
  }
  return false;
}

bool AdditionalResultsType::HasVector(const Expression exactOutput) {
  Context *globalContext =
      AppsContainerHelper::sharedAppsContainerGlobalContext();
  Expression norm =
      VectorHelper::BuildVectorNorm(exactOutput.clone(), globalContext);
  if (norm.isUninitialized()) {
    return false;
  }
  assert(!norm.isUndefined());
  int nChildren = exactOutput.numberOfChildren();
  for (int i = 0; i < nChildren; ++i) {
    if (exactOutput.childAtIndex(i).isScalarComplex(
            Preferences::sharedPreferences)) {
      return false;
    }
  }
  return true;
}

bool AdditionalResultsType::HasMatrix(const Expression exactOutput) {
  assert(!exactOutput.isUninitialized());
  assert(!exactOutput.hasUnit());
  return exactOutput.type() == ExpressionNode::Type::Matrix &&
         !exactOutput.recursivelyMatches(Expression::IsUndefined);
}

static bool expressionIsInterestingFunction(const Expression e) {
  assert(!e.isUninitialized());
  if (e.isOfType({ExpressionNode::Type::Opposite,
                  ExpressionNode::Type::Parenthesis})) {
    return expressionIsInterestingFunction(e.childAtIndex(0));
  }
  return !e.isNumber() &&
         !e.isOfType({ExpressionNode::Type::ConstantMaths,
                      ExpressionNode::Type::UnitConvert}) &&
         !e.recursivelyMatches(Expression::IsSequence) &&
         e.numberOfNumericalValues() == 1;
}

bool AdditionalResultsType::HasFunction(const Expression input,
                                        const Expression approximateOutput) {
  // We want a single numerical value and to avoid showing the identity function
  assert(!input.isUninitialized());
  assert(!approximateOutput.isUndefined());
  assert(!approximateOutput.hasUnit());
  assert(approximateOutput.type() != ExpressionNode::Type::Matrix);
  return approximateOutput.type() != ExpressionNode::Type::Nonreal &&
         expressionIsInterestingFunction(input);
}

bool AdditionalResultsType::HasScientificNotation(
    const Expression approximateOutput) {
  assert(!approximateOutput.hasUnit());
  assert(approximateOutput.type() != ExpressionNode::Type::Matrix);
  Context *globalContext =
      AppsContainerHelper::sharedAppsContainerGlobalContext();
  return approximateOutput.type() != ExpressionNode::Type::Nonreal &&
         Preferences::sharedPreferences->displayMode() !=
             Preferences::PrintFloatMode::Scientific &&
         ScientificNotationHelper::HasAdditionalOutputs(approximateOutput,
                                                        globalContext);
}

bool AdditionalResultsType::HasInteger(const Expression exactOutput) {
  assert(!exactOutput.isUninitialized());
  assert(!exactOutput.hasUnit());
  constexpr const char *k_maximalIntegerWithAdditionalResults =
      "10000000000000000";
  return exactOutput.isBasedIntegerCappedBy(
      k_maximalIntegerWithAdditionalResults);
}

bool AdditionalResultsType::HasRational(const Expression exactOutput) {
  // Find forms like [12]/[23] or -[12]/[23]
  assert(!exactOutput.isUninitialized());
  assert(!exactOutput.hasUnit());
  return exactOutput.isDivisionOfIntegers() ||
         (exactOutput.type() == ExpressionNode::Type::Opposite &&
          exactOutput.childAtIndex(0).isDivisionOfIntegers());
}

}  // namespace Calculation
