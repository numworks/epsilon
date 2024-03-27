#include "calculation.h"

#include <apps/apps_container_helper.h>
#include <apps/shared/expression_display_permissions.h>
#include <poincare/exception_checkpoint.h>
#include <poincare/nonreal.h>
#include <poincare/undefined.h>
#include <string.h>

#include <algorithm>
#include <cmath>

#include "app.h"
#include "poincare/expression_node.h"

using namespace Poincare;
using namespace Shared;

namespace Calculation {

bool Calculation::operator==(const Calculation &c) {
  return inputTreeSize() == c.inputTreeSize() &&
         memcmp(inputTree(), c.inputTree(), c.inputTreeSize()) == 0 &&
         strcmp(approximateOutputText(NumberOfSignificantDigits::Maximal),
                c.approximateOutputText(NumberOfSignificantDigits::Maximal)) ==
             0 &&
         strcmp(approximateOutputText(NumberOfSignificantDigits::UserDefined),
                c.approximateOutputText(
                    NumberOfSignificantDigits::UserDefined)) == 0
         /* Some calculations can make appear trigonometric functions in their
          * exact output. Their argument will be different with the angle unit
          * preferences but both input and approximate output will be the same.
          * For example, i^(sqrt(3)) = cos(sqrt(3)*pi/2)+i*sin(sqrt(3)*pi/2) if
          * angle unit is radian and i^(sqrt(3)) =
          * cos(sqrt(3)*90+i*sin(sqrt(3)*90) in degree. */
         && strcmp(exactOutputText(), c.exactOutputText()) == 0;
}

Calculation *Calculation::next() const {
  const char *result =
      reinterpret_cast<const char *>(this) + sizeof(Calculation);
  for (int i = 0; i < k_numberOfExpressions; i++) {
    // Pass inputText, exactOutputText, ApproximateOutputText x2
    result = result + strlen(result) + 1;
  }
  return reinterpret_cast<Calculation *>(const_cast<char *>(result));
}

const char *Calculation::approximateOutputText(
    NumberOfSignificantDigits numberOfSignificantDigits) const {
  const char *exactOutput = exactOutputText();
  const char *approximateOutputTextWithMaxNumberOfDigits =
      exactOutput + strlen(exactOutput) + 1;
  if (numberOfSignificantDigits == NumberOfSignificantDigits::Maximal) {
    return approximateOutputTextWithMaxNumberOfDigits;
  }
  return approximateOutputTextWithMaxNumberOfDigits +
         strlen(approximateOutputTextWithMaxNumberOfDigits) + 1;
}

Expression Calculation::input() {
  Expression e = JuniorExpression::Builder(inputTree());
  assert(!e.isUninitialized());
  return e;
}

Expression Calculation::exactOutput() {
  /* Because the angle unit might have changed, we do not simplify again. We
   * thereby avoid turning cos(Pi/4) into sqrt(2)/2 and displaying
   * 'sqrt(2)/2 = 0.999906' (which is totally wrong) instead of
   * 'cos(pi/4) = 0.999906' (which is true in degree). */
  Expression e = Expression::Parse(exactOutputText(), nullptr);
  assert(!e.isUninitialized());
  return e;
}

Expression Calculation::approximateOutput(
    NumberOfSignificantDigits numberOfSignificantDigits) {
  // clang-format off
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
  // clang-format on
  Expression e = Expression::Parse(
      approximateOutputText(numberOfSignificantDigits), nullptr);
  assert(!e.isUninitialized());
  return e;
}

Layout Calculation::createInputLayout() {
  ExceptionCheckpoint ecp;
  if (ExceptionRun(ecp)) {
    Expression e = input();
    if (!e.isUninitialized()) {
      return e.createLayout(Preferences::PrintFloatMode::Decimal,
                            PrintFloat::k_maxNumberOfSignificantDigits,
                            App::app()->localContext());
    }
  }
  return Layout();
}

Layout Calculation::createExactOutputLayout(bool *couldNotCreateExactLayout) {
  ExceptionCheckpoint ecp;
  if (ExceptionRun(ecp)) {
    Expression e = exactOutput();
    if (!e.isUninitialized()) {
      return e.createLayout(Preferences::PrintFloatMode::Decimal,
                            PrintFloat::k_maxNumberOfSignificantDigits,
                            App::app()->localContext());
    }
  }
  *couldNotCreateExactLayout = true;
  return Layout();
}

Layout Calculation::createApproximateOutputLayout(
    bool *couldNotCreateApproximateLayout) {
  ExceptionCheckpoint ecp;
  if (ExceptionRun(ecp)) {
    Expression e = approximateOutput(NumberOfSignificantDigits::UserDefined);
    if (!e.isUninitialized()) {
      return e.createLayout(displayMode(), numberOfSignificantDigits(),
                            App::app()->localContext());
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

static bool ShouldOnlyDisplayExactOutput(Expression input) {
  /* If the input is a "store in a function", do not display the approximate
   * result. This prevents x->f(x) from displaying x = undef. */
  assert(!input.isUninitialized());
  return input.type() == ExpressionNode::Type::Store &&
         input.childAtIndex(1).type() == ExpressionNode::Type::Function;
}

Calculation::DisplayOutput Calculation::displayOutput(Context *context) {
  if (m_displayOutput != DisplayOutput::Unknown) {
    return m_displayOutput;
  }
  Expression inputExp = input();
  Expression outputExp = exactOutput();
  const char *exactText = exactOutputText();
  const char *approxText =
      approximateOutputText(NumberOfSignificantDigits::UserDefined);
  if (inputExp.isUninitialized() || outputExp.isUninitialized() ||
      ShouldOnlyDisplayExactOutput(inputExp)) {
    m_displayOutput = DisplayOutput::ExactOnly;
  } else if (
      // If the exact and approximate outputs are equal
      strcmp(approxText, exactText) == 0 ||
      // If the exact result is 'undef'
      strcmp(exactText, Undefined::Name()) == 0 ||
      // If the approximate output is 'nonreal'
      strcmp(approxText, Nonreal::Name()) == 0 ||
      // Other conditions are factorized in ExpressionDisplayPermissions
      ExpressionDisplayPermissions::ShouldOnlyDisplayApproximation(
          inputExp, outputExp,
          approximateOutput(NumberOfSignificantDigits::UserDefined), context)) {
    m_displayOutput = DisplayOutput::ApproximateOnly;
  } else if (
      // If the approximate output is 'undef'
      strcmp(approxText, Undefined::Name()) == 0) {
    // TODO_PCJ: This allow the display of exact pcj results, regulate it.
    m_displayOutput = DisplayOutput::ExactOnly;
  } else if (inputExp.isIdenticalTo(outputExp) ||
             inputExp.recursivelyMatches(Expression::IsApproximate, context) ||
             outputExp.recursivelyMatches(Expression::IsApproximate, context) ||
             inputExp.recursivelyMatches(Expression::IsPercent, context)) {
    m_displayOutput = DisplayOutput::ExactAndApproximateToggle;
  } else {
    m_displayOutput = DisplayOutput::ExactAndApproximate;
  }
  return m_displayOutput;
}

void Calculation::createOutputLayouts(Layout *exactOutput,
                                      Layout *approximateOutput,
                                      Context *context,
                                      bool canChangeDisplayOutput,
                                      KDCoordinate maxVisibleWidth,
                                      KDFont::Size font) {
  assert(exactOutput && approximateOutput);

  // Create the exact output layout
  *exactOutput = Layout();
  if (DisplaysExact(displayOutput(context))) {
    bool couldNotCreateExactLayout = false;
    *exactOutput = createExactOutputLayout(&couldNotCreateExactLayout);
    if (couldNotCreateExactLayout) {
      if (canChangeDisplayOutput &&
          displayOutput(context) != DisplayOutput::ExactOnly) {
        forceDisplayOutput(DisplayOutput::ApproximateOnly);
      } else {
        /* We should only display the exact result, but we cannot create it
         * -> raise an exception. */
        ExceptionCheckpoint::Raise();
      }
    }
    if (canChangeDisplayOutput &&
        displayOutput(context) == DisplayOutput::ExactAndApproximate &&
        (*exactOutput)->layoutSize(font).width() > maxVisibleWidth) {
      forceDisplayOutput(DisplayOutput::ExactAndApproximateToggle);
    }
  }

  // Create the approximate output layout
  if (displayOutput(context) == DisplayOutput::ExactOnly) {
    *approximateOutput = *exactOutput;
  } else {
    bool couldNotCreateApproximateLayout = false;
    *approximateOutput =
        createApproximateOutputLayout(&couldNotCreateApproximateLayout);
    if (couldNotCreateApproximateLayout) {
      if (canChangeDisplayOutput &&
          displayOutput(context) != DisplayOutput::ApproximateOnly) {
        /* Set display the output to ApproximateOnly, make room in the pool by
         * erasing the exact layout, retry to create the approximate layout. */
        forceDisplayOutput(DisplayOutput::ApproximateOnly);
        *exactOutput = Layout();
        couldNotCreateApproximateLayout = false;
        *approximateOutput =
            createApproximateOutputLayout(&couldNotCreateApproximateLayout);
        if (couldNotCreateApproximateLayout) {
          ExceptionCheckpoint::Raise();
        }
      } else {
        ExceptionCheckpoint::Raise();
      }
    }
  }
}

Calculation::EqualSign Calculation::equalSign(Context *context) {
  // TODO: implement a UserCircuitBreaker
  if (m_equalSign != EqualSign::Unknown) {
    return m_equalSign;
  }
  if (m_displayOutput == DisplayOutput::ExactOnly ||
      m_displayOutput == DisplayOutput::ApproximateOnly) {
    /* Do not compute the equal sign if not needed.
     * We don't override m_equalSign here in case it needs to be computed later
     * */
    return EqualSign::Approximation;
  }
  /* Displaying the right equal symbol is less important than displaying a
   * result, so we do not want equalSign to create a pool failure that would
   * prevent from displaying a result that we managed to compute. We thus
   * encapsulate the method in an exception checkpoint: if there was not enough
   * memory on the pool to compute the equal sign, just return
   * EqualSign::Approximation. We can safely use an exception checkpoint here
   * because we are sure of not modifying any pre-existing node in the pool. We
   * are sure there cannot be a Store in the exactOutput. */
  ExceptionCheckpoint ecp;
  if (ExceptionRun(ecp)) {
    Expression exactOutputExpression = exactOutput();
    if (input().recursivelyMatches(
            [](const Expression e) {
              return Expression::IsPercent(e) ||
                     e.type() == ExpressionNode::Type::Factor;
            },
            context)) {
      /* When the input contains percent or factor, the exact expression is not
       * fully reduced so we need to reduce it again prior to computing equal
       * sign */
      PoincareHelpers::CloneAndSimplify(
          &exactOutputExpression, context,
          {.complexFormat = complexFormat(),
           .angleUnit = angleUnit(),
           .symbolicComputation = SymbolicComputation::
               ReplaceAllSymbolsWithDefinitionsOrUndefined});
    }
    m_equalSign = Expression::ExactAndApproximateExpressionsAreEqual(
                      exactOutputExpression,
                      approximateOutput(NumberOfSignificantDigits::UserDefined))
                      ? EqualSign::Equal
                      : EqualSign::Approximation;
    return m_equalSign;
  } else {
    /* Do not override m_equalSign in case there is enough room in the pool
     * later to compute it. */
    return EqualSign::Approximation;
  }
}

void Calculation::fillExpressionsForAdditionalResults(
    Expression *input, Expression *exactOutput, Expression *approximateOutput) {
  Context *globalContext =
      AppsContainerHelper::sharedAppsContainerGlobalContext();
  *input = this->input();
  *approximateOutput =
      this->approximateOutput(NumberOfSignificantDigits::Maximal);
  *exactOutput = DisplaysExact(displayOutput(globalContext))
                     ? this->exactOutput()
                     : *approximateOutput;
}

AdditionalResultsType Calculation::additionalResultsType() {
  if (m_additionalResultsType.isUninitialized()) {
    Expression i, a, e;
    fillExpressionsForAdditionalResults(&i, &e, &a);
    m_additionalResultsType =
        AdditionalResultsType::AdditionalResultsForExpressions(
            i, e, a, m_calculationPreferences);
  }
  assert(!m_additionalResultsType.isUninitialized());
  return m_additionalResultsType;
}

}  // namespace Calculation
