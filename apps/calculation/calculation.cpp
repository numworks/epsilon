#include "app.h"
#include "additional_outputs/scientific_notation_helper.h"
#include "calculation.h"
#include "poincare/expression_node.h"
#include <apps/apps_container_helper.h>
#include <apps/global_preferences.h>
#include <apps/exam_mode_configuration.h>
#include <apps/shared/poincare_helpers.h>
#include <apps/shared/scrollable_multiple_expressions_view.h>
#include <apps/shared/utils.h>
#include <poincare/exception_checkpoint.h>
#include <poincare/matrix.h>
#include <poincare/undefined.h>
#include <poincare/unit.h>
#include <poincare/nonreal.h>
#include <poincare/trigonometry.h>
#include <string.h>
#include <cmath>
#include <algorithm>

using namespace Poincare;
using namespace Shared;

namespace Calculation {

bool Calculation::operator==(const Calculation& c) {
  return strcmp(inputText(), c.inputText()) == 0
      && strcmp(approximateOutputText(NumberOfSignificantDigits::Maximal), c.approximateOutputText(NumberOfSignificantDigits::Maximal)) == 0
      && strcmp(approximateOutputText(NumberOfSignificantDigits::UserDefined), c.approximateOutputText(NumberOfSignificantDigits::UserDefined)) == 0
      /* Some calculations can make appear trigonometric functions in their
       * exact output. Their argument will be different with the angle unit
       * preferences but both input and approximate output will be the same.
       * For example, i^(sqrt(3)) = cos(sqrt(3)*pi/2)+i*sin(sqrt(3)*pi/2) if
       * angle unit is radian and i^(sqrt(3)) = cos(sqrt(3)*90+i*sin(sqrt(3)*90)
       * in degree. */
      && strcmp(exactOutputText(), c.exactOutputText()) == 0;
}

Calculation * Calculation::next() const {
  const char * result = reinterpret_cast<const char *>(this) + sizeof(Calculation);
  for (int i = 0; i < k_numberOfExpressions; i++) {
    result = result + strlen(result) + 1; // Pass inputText, exactOutputText, ApproximateOutputText x2
  }
  return reinterpret_cast<Calculation *>(const_cast<char *>(result));
}

const char * Calculation::approximateOutputText(NumberOfSignificantDigits numberOfSignificantDigits) const {
  const char * exactOutput = exactOutputText();
  const char * approximateOutputTextWithMaxNumberOfDigits = exactOutput + strlen(exactOutput) + 1;
  if (numberOfSignificantDigits == NumberOfSignificantDigits::Maximal) {
    return approximateOutputTextWithMaxNumberOfDigits;
  }
  return approximateOutputTextWithMaxNumberOfDigits + strlen(approximateOutputTextWithMaxNumberOfDigits) + 1;
}

Expression Calculation::input() {
  return Expression::Parse(m_inputText, nullptr);
}

Expression Calculation::exactOutput() {
  /* Because the angle unit might have changed, we do not simplify again. We
   * thereby avoid turning cos(Pi/4) into sqrt(2)/2 and displaying
   * 'sqrt(2)/2 = 0.999906' (which is totally wrong) instead of
   * 'cos(pi/4) = 0.999906' (which is true in degree). */
  return Expression::Parse(exactOutputText(), nullptr);
}

Expression Calculation::approximateOutput(NumberOfSignificantDigits numberOfSignificantDigits) {
  /* Warning:
   * Since quite old versions of Epsilon, the Expression 'exp' was used to be
   * approximated again to ensure its content was in the expected form - a
   * linear combination of Decimal.
   * However, since the approximate output may contain units and that a
   * Poincare::Unit approximates to undef, thus it must not be approximated
   * anymore.
   * We have to keep two serializations of the approximation outputs:
   * - one with the maximal significant digits, to be used by 'Ans' or when
   *   handling 'OK' event on the approximation output.
   * - one with the displayed number of significant digits that we parse to
   *   create the displayed layout. If we used the other serialization to
   *   create the layout, the result of the parsing could be an Integer which
   *   does not take the number of significant digits into account when creating
   *   its layout. This would lead to wrong number of significant digits in the
   *   layout.
   *   For instance:
   *        Number of asked significant digits: 7
   *        Input: "123456780", Approximate output: "1.234567E8"
   *
   *  |--------------------------------------------------------------------------------------|
   *  | Number of significant digits | Approximate text | Parse expression    | Layout       |
   *  |------------------------------+------------------+---------------------+--------------|
   *  | Maximal                      | "123456780"      | Integer(123456780)  | "123456780"  |
   *  |------------------------------+------------------+---------------------+--------------|
   *  | User defined                 | "1.234567E8"     | Decimal(1.234567E8) | "1.234567E8" |
   *  |--------------------------------------------------------------------------------------|
   *
   */
  return Expression::Parse(approximateOutputText(numberOfSignificantDigits), nullptr);
}

Layout Calculation::createInputLayout() {
  Poincare::ExceptionCheckpoint ecp;
  if (ExceptionRun(ecp)) {
    Expression e = input();
    if (!e.isUninitialized()) {
      return e.createLayout(Preferences::PrintFloatMode::Decimal, PrintFloat::k_numberOfStoredSignificantDigits, App::app()->localContext());
    }
  }
  return Layout();
}

Layout Calculation::createExactOutputLayout(bool * couldNotCreateExactLayout) {
  Poincare::ExceptionCheckpoint ecp;
  if (ExceptionRun(ecp)) {
    Expression e = exactOutput();
    if (!e.isUninitialized()) {
      return e.createLayout(Poincare::Preferences::PrintFloatMode::Decimal, PrintFloat::k_numberOfStoredSignificantDigits, App::app()->localContext());
    }
  }
  *couldNotCreateExactLayout = true;
  return Layout();
}

Layout Calculation::createApproximateOutputLayout(bool * couldNotCreateApproximateLayout) {
  Poincare::ExceptionCheckpoint ecp;
  if (ExceptionRun(ecp)) {
    Expression e = approximateOutput(NumberOfSignificantDigits::UserDefined);
    if (!e.isUninitialized()) {
      return PoincareHelpers::CreateLayout(e, App::app()->localContext());
    }
  }
  *couldNotCreateApproximateLayout = true;
  return Layout();
}

KDCoordinate Calculation::height(bool expanded) {
  KDCoordinate h = expanded ? m_expandedHeight : m_height;
  assert(h >= 0);
  return h;
}

void Calculation::setHeights(KDCoordinate height, KDCoordinate expandedHeight) {
  m_height = height;
  m_expandedHeight = expandedHeight;
}

Calculation::DisplayOutput Calculation::displayOutput(Context * context) {
  if (m_displayOutput != DisplayOutput::Unknown) {
    return m_displayOutput;
  }
  Expression inputExp = input();
  Expression outputExp = exactOutput();
  if (inputExp.isUninitialized()
   || outputExp.isUninitialized()
   || Utils::ShouldOnlyDisplayExactOutput(inputExp))
  {
    m_displayOutput = DisplayOutput::ExactOnly;
  } else if (
      /* If the exact and approximate outputs are equal (with the
       * UserDefined number of significant digits), do not display the exact
       * output. Indeed, in this case, the layouts are identical. */
      strcmp(exactOutputText(), approximateOutputText(NumberOfSignificantDigits::UserDefined)) == 0
      // If the approximate output is 'nonreal' or the exact result is 'undef'
   || strcmp(exactOutputText(), Undefined::Name()) == 0
   || strcmp(approximateOutputText(NumberOfSignificantDigits::Maximal), Nonreal::Name()) == 0
      /* If the approximate output is 'undef' and the input and exactOutput are
       * equal */
   || (strcmp(approximateOutputText(NumberOfSignificantDigits::Maximal), Undefined::Name()) == 0
    && strcmp(inputText(), exactOutputText()) == 0)
      // All other conditions are factorized within PoincareHelpers
   || Utils::ShouldOnlyDisplayApproximation(inputExp, outputExp, context))
  {
    m_displayOutput = DisplayOutput::ApproximateOnly;
  } else if (inputExp.recursivelyMatches(Expression::IsApproximate, context)
      || outputExp.recursivelyMatches(Expression::IsApproximate, context)
      || inputExp.recursivelyMatches(Expression::IsPercent, context))
  {
    m_displayOutput = DisplayOutput::ExactAndApproximateToggle;
  } else {
    m_displayOutput = DisplayOutput::ExactAndApproximate;
  }
  return m_displayOutput;
}

void Calculation::forceDisplayOutput(DisplayOutput d) {
  // Heights haven't been computed yet
  assert(m_height == -1 && m_expandedHeight == -1);
  m_displayOutput = d;
}

Calculation::EqualSign Calculation::exactAndApproximateDisplayedOutputsAreEqual(Poincare::Context * context) {
  // TODO: implement a UserCircuitBreaker
  if (m_equalSign != EqualSign::Unknown) {
    return m_equalSign;
  }
  if (m_displayOutput == DisplayOutput::ExactOnly || m_displayOutput == DisplayOutput::ApproximateOnly) {
    /* Do not compute the equal sign if not needed.
     * We don't override m_equalSign here in case it needs to be computed later
     * */
    return EqualSign::Approximation;
  }
  /* Displaying the right equal symbol is less important than displaying a
   * result, so we do not want exactAndApproximateDisplayedOutputsAreEqual to
   * create a pool failure that would prevent from displaying a result that we
   * managed to compute. We thus encapsulate the method in an exception
   * checkpoint: if there was not enough memory on the pool to compute the equal
   * sign, just return EqualSign::Approximation.
   * We can safely use an exception checkpoint here because we are sure of not
   * modifying any pre-existing node in the pool. We are sure there cannot be a
   * Store in the exactOutput. */
  Poincare::ExceptionCheckpoint ecp;
  if (ExceptionRun(ecp)) {
    Expression exactOutputExpression = exactOutput();
    if (input().recursivelyMatches(Expression::IsPercent, context)) {
      /* When the input contains percent, the exact expression is not fully
       * reduced so we need to reduce it again prior to computing equal sign */
      PoincareHelpers::CloneAndSimplify(&exactOutputExpression, context, Poincare::ExpressionNode::ReductionTarget::User, ExpressionNode::SymbolicComputation::ReplaceAllSymbolsWithDefinitionsOrUndefined);
    }
    m_equalSign = Expression::ExactAndApproximateExpressionsAreEqual(exactOutputExpression, approximateOutput(NumberOfSignificantDigits::UserDefined)) ? EqualSign::Equal : EqualSign::Approximation;
    return m_equalSign;
  } else {
    /* Do not override m_equalSign in case there is enough room in the pool
     * later to compute it. */
    return EqualSign::Approximation;
  }
}

Calculation::AdditionalInformations Calculation::additionalInformations() {
  if (ExamModeConfiguration::additionalResultsAreForbidden()
      || strcmp(approximateOutputText(NumberOfSignificantDigits::Maximal), "undef") == 0) {
    return AdditionalInformations();
  }
  Preferences * preferences = Preferences::sharedPreferences();
  Expression i = input();
  Expression o = exactOutput();
  Expression a = approximateOutput(NumberOfSignificantDigits::Maximal);
  /* Special case for Store:
   * Store nodes have to be at the root of the expression, which prevents
   * from creating new expressions with store node as a child. We don't
   * return any additional outputs for them to avoid bothering with special
   * cases. */
  if (i.isUninitialized() || o.isUninitialized() || i.type() == ExpressionNode::Type::Store || o.type() == ExpressionNode::Type::List || a.type() == ExpressionNode::Type::List || a.isUndefined() || a.recursivelyMatches([](const Expression e, Context * c) { return e.isOfType({ ExpressionNode::Type::Infinity }); }, nullptr)) {
    return AdditionalInformations {};
  }
  /* Using the approximated output instead of the user input to guess the
   * complex format makes additional results more consistent when the user has
   * created complexes in Complex mode and then switched back to Real mode. */
  Preferences::ComplexFormat complexFormat = Preferences::UpdatedComplexFormatWithExpressionInput(preferences->complexFormat(), a, nullptr);
  bool isComplex = a.hasDefinedComplexApproximation(nullptr, complexFormat, preferences->angleUnit());
  /* Trigonometry additional results are displayed if either input or output is
   * a direct or inverse trigonometric function. Indeed, we want to capture both
   * cases:
   *
   * - > input: cos(60)
   *   > output: 1/2
   * - > input: 2cos(2) - cos(2)
   *   > output: cos(2)
   * However if the result is complex, it is treated as a complex result instead
   */
  Context * globalContext = AppsContainerHelper::sharedAppsContainerGlobalContext();
  if (!isComplex) {
    if (Trigonometry::isInverseTrigonometryFunction(i) || Trigonometry::isInverseTrigonometryFunction(o)) {
      // The angle cannot be complex since Expression a isn't
      return AdditionalInformations {.inverseTrigonometry = true};
    }
    Expression directExpression;
    if (Trigonometry::isDirectTrigonometryFunction(i)) {
      directExpression = i;
    } else if (Trigonometry::isDirectTrigonometryFunction(o)) {
      directExpression = o;
    }
    if (!directExpression.isUninitialized() && !directExpression.childAtIndex(0).hasDefinedComplexApproximation(globalContext, Preferences::ComplexFormat::Cartesian, preferences->angleUnit())) {
      // The angle must be real.
      return AdditionalInformations {.directTrigonometry = true};
    }
  }
  if (o.hasUnit()) {
    AdditionalInformations additionalInformations = {};
    Expression unit;
    PoincareHelpers::ReduceAndRemoveUnit(&o, globalContext, ExpressionNode::ReductionTarget::User, &unit, ExpressionNode::SymbolicComputation::ReplaceAllSymbolsWithDefinitionsOrUndefined, ExpressionNode::UnitConversion::None);
    double value = PoincareHelpers::ApproximateToScalar<double>(o, globalContext);
    if (Unit::ShouldDisplayAdditionalOutputs(value, unit, GlobalPreferences::sharedGlobalPreferences()->unitFormat())
        || UnitComparison::ShouldDisplayUnitComparison(value, unit)) {
      additionalInformations.unit = true;
    }
    return additionalInformations;
  }
  if (o.type() == ExpressionNode::Type::Matrix) {
    if (static_cast<const Matrix&>(o).vectorType() != Array::VectorType::None) {
      return AdditionalInformations {.vector = true};
    }
    return AdditionalInformations {.matrix = true};
  }
  if (isComplex) {
    return AdditionalInformations {.complex = true};
  }
  AdditionalInformations additionalInformations = {};
  if (a.type() != ExpressionNode::Type::Nonreal && preferences->displayMode() != Preferences::PrintFloatMode::Scientific) {
    // There should be no units at this point
    assert(!a.hasUnit());
    additionalInformations.scientificNotation = ScientificNotationHelper::HasAdditionalOutputs(a, globalContext);
  }
  // We want a single numerical value and to avoid showing the identity function
  bool isInterestingFunction = !i.isNumber() && i.type() != ExpressionNode::Type::ConstantMaths && !(i.type() == ExpressionNode::Type::Opposite && (i.childAtIndex(0).isNumber() || i.childAtIndex(0).type() == ExpressionNode::Type::ConstantMaths));
  assert(!a.isUndefined());
  if (isInterestingFunction && a.type() != ExpressionNode::Type::Nonreal && i.numberOfNumericalValues() == 1) {
    additionalInformations.function = true;
  }
  if (o.isBasedIntegerCappedBy(k_maximalIntegerWithAdditionalInformation)) {
    additionalInformations.integer = true;
    return additionalInformations;
  }
  // Find forms like [12]/[23] or -[12]/[23]
  if (o.isDivisionOfIntegers() || (o.type() == ExpressionNode::Type::Opposite && o.childAtIndex(0).isDivisionOfIntegers())) {
    additionalInformations.rational = true;
    return additionalInformations;
  }
  return additionalInformations;
}

}
