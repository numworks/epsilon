#include "calculation.h"
#include "../shared/poincare_helpers.h"
#include "../shared/scrollable_multiple_expressions_view.h"
#include "../global_preferences.h"
#include "../exam_mode_configuration.h"
#include "app.h"
#include <poincare/exception_checkpoint.h>
#include <poincare/undefined.h>
#include <poincare/unit.h>
#include <poincare/nonreal.h>
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
      return e.createLayout(Preferences::PrintFloatMode::Decimal, PrintFloat::k_numberOfStoredSignificantDigits);
    }
  }
  return Layout();
}

Layout Calculation::createExactOutputLayout(bool * couldNotCreateExactLayout) {
  Poincare::ExceptionCheckpoint ecp;
  if (ExceptionRun(ecp)) {
    Expression e = exactOutput();
    if (!e.isUninitialized()) {
      return e.createLayout(Poincare::Preferences::PrintFloatMode::Decimal, PrintFloat::k_numberOfStoredSignificantDigits);
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
      return PoincareHelpers::CreateLayout(e);
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
   || shouldOnlyDisplayExactOutput())
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
      // Exact output with remaining dependency are not displayed to avoid 2 ≈ undef
   || outputExp.type() == ExpressionNode::Type::Dependency
      // Lists or Matrices with only nonreal/undefined children
   || (outputExp.isOfType({ExpressionNode::Type::List, ExpressionNode::Type::Matrix}) && outputExp.allChildrenAreUndefined())
      // Force all outputs to be ApproximateOnly if required by the exam mode configuration
   || ExamModeConfiguration::exactExpressionIsForbidden(outputExp)
      /* If the input contains the following types, we only display the
       * approximate output. */
   || inputExp.recursivelyMatches(
        [](const Expression e, Context * c) {
          return e.isOfType({
            ExpressionNode::Type::ConstantPhysics,
            ExpressionNode::Type::Randint,
            ExpressionNode::Type::Random,
            ExpressionNode::Type::Unit,
            ExpressionNode::Type::Round,
            ExpressionNode::Type::FracPart,
            ExpressionNode::Type::Integral,
            ExpressionNode::Type::Product,
            ExpressionNode::Type::Sum,
            ExpressionNode::Type::Derivative,
            ExpressionNode::Type::Sequence,
	    ExpressionNode::Type::DistributionDispatcher,
          });
        }, context))
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

bool Calculation::shouldOnlyDisplayExactOutput() {
  /* If the input is a "store in a function", do not display the approximate
   * result. This prevents x->f(x) from displaying x = undef. */
  Expression i = input();
  assert(!i.isUninitialized());
  return i.type() == ExpressionNode::Type::Store
    && i.childAtIndex(1).type() == ExpressionNode::Type::Function;
}

Calculation::EqualSign Calculation::exactAndApproximateDisplayedOutputsAreEqual(Poincare::Context * context) {
  // TODO: implement a UserCircuitBreaker
  if (m_equalSign != EqualSign::Unknown) {
    return m_equalSign;
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
    Preferences * preferences = Preferences::sharedPreferences();
    // TODO: complex format should not be needed here (as it is not used to create layouts)
    m_equalSign = Expression::ParsedExpressionsAreEqual(exactOutputText(), approximateOutputText(NumberOfSignificantDigits::UserDefined), context, preferences->complexFormat(), preferences->angleUnit(), GlobalPreferences::sharedGlobalPreferences()->unitFormat()) ? EqualSign::Equal : EqualSign::Approximation;
    return m_equalSign;
  } else {
    /* Do not override m_equalSign in case there is enough room in the pool
     * later to compute it. */
    return EqualSign::Approximation;
  }
}

Calculation::AdditionalInformationType Calculation::additionalInformationType() {
  if (ExamModeConfiguration::additionalResultsAreForbidden()
      || strcmp(approximateOutputText(NumberOfSignificantDigits::Maximal), "undef") == 0) {
    return AdditionalInformationType::None;
  }
  Preferences * preferences = Preferences::sharedPreferences();
  Expression i = input();
  Expression o = exactOutput();
  Expression a = approximateOutput(NumberOfSignificantDigits::Maximal);
  Preferences::ComplexFormat complexFormat =  Expression::UpdatedComplexFormatWithExpressionInput(preferences->complexFormat(), a, nullptr);
  bool isComplex = a.hasDefinedComplexApproximation(nullptr, complexFormat, preferences->angleUnit());
  /* Special case for Equal and Store:
   * Equal/Store nodes have to be at the root of the expression, which prevents
   * from creating new expressions with equal/store node as a child. We don't
   * return any additional outputs for them to avoid bothering with special
   * cases. */
  if (i.isUninitialized() || o.isUninitialized() || i.type() == ExpressionNode::Type::Equal || i.type() == ExpressionNode::Type::Store || a.type() == ExpressionNode::Type::Undefined) {
    return AdditionalInformationType::None;
  }
  /* Trigonometry additional results are displayed if either input or output is a sin or a cos. Indeed, we want to capture both cases:
   * - > input: cos(60)
   *   > output: 1/2
   * - > input: 2cos(2) - cos(2)
   *   > output: cos(2)
   * However if the result is complex, it is treated as a complex result instead
   */
  if (!isComplex && (i.isOfType({ExpressionNode::Type::Cosine, ExpressionNode::Type::Sine}) || o.isOfType({ExpressionNode::Type::Cosine, ExpressionNode::Type::Sine}))) {
    return AdditionalInformationType::Trigonometry;
  }
  if (o.hasUnit()) {
    Expression unit;
    PoincareHelpers::ReduceAndRemoveUnit(&o, App::app()->localContext(), ExpressionNode::ReductionTarget::User, &unit, ExpressionNode::SymbolicComputation::ReplaceAllSymbolsWithDefinitionsOrUndefined, ExpressionNode::UnitConversion::None);
    double value = PoincareHelpers::ApproximateToScalar<double>(o, App::app()->localContext());
    if (Unit::ShouldDisplayAdditionalOutputs(value, unit, GlobalPreferences::sharedGlobalPreferences()->unitFormat())
        || UnitComparison::ShouldDisplayUnitComparison(value, unit)) {
      return AdditionalInformationType::Unit;
    }
    return AdditionalInformationType::None;
  }
  if (o.isBasedIntegerCappedBy(k_maximalIntegerWithAdditionalInformation)) {
    return AdditionalInformationType::Integer;
  }
  // Find forms like [12]/[23] or -[12]/[23]
  if (o.isDivisionOfIntegers() || (o.type() == ExpressionNode::Type::Opposite && o.childAtIndex(0).isDivisionOfIntegers())) {
    return AdditionalInformationType::Rational;
  }
  if (isComplex) {
    return AdditionalInformationType::Complex;
  }
  if (o.type() == ExpressionNode::Type::Matrix) {
    return AdditionalInformationType::Matrix;
  }
  return AdditionalInformationType::None;
}

}
