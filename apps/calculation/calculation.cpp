#include "calculation.h"

#include <poincare/cas.h>
#include <poincare/helpers/expression_equal_sign.h>
#include <poincare/old/exception_checkpoint.h>
#include <poincare/src/expression/projection.h>
#include <string.h>

#include <algorithm>
#include <cmath>

using namespace Poincare;
using namespace Shared;

namespace Calculation {

bool Calculation::operator==(const Calculation& c) {
  /* Some calculations can make appear trigonometric functions in their exact
   * output. Their argument will be different with the angle unit preferences
   * but both input and approximate output will be the same.
   * For example, i^(sqrt(3)) = cos(sqrt(3)*pi/2)+i*sin(sqrt(3)*pi/2) if angle
   * unit is radian and i^(sqrt(3)) = cos(sqrt(3)*90+i*sin(sqrt(3)*90) in
   * degree. */
  return cumulatedTreeSizes() == c.cumulatedTreeSizes() &&
         memcmp(m_trees, c.m_trees, cumulatedTreeSizes());
}

Calculation* Calculation::next() const {
  return reinterpret_cast<Calculation*>(
      const_cast<char*>(reinterpret_cast<const char*>(this) +
                        sizeof(Calculation) + cumulatedTreeSizes()));
}

UserExpression Calculation::input() {
  UserExpression e = NewExpression::Builder(inputTree());
  assert(!e.isUninitialized());
  return e;
}

UserExpression Calculation::exactOutput() {
  /* Because the angle unit might have changed, we do not simplify again. We
   * thereby avoid turning cos(Pi/4) into sqrt(2)/2 and displaying
   * 'sqrt(2)/2 = 0.999906' (which is totally wrong) instead of
   * 'cos(pi/4) = 0.999906' (which is true in degree). */
  UserExpression e = UserExpression::Builder(exactOutputTree());
  assert(!e.isUninitialized());
  return e;
}

UserExpression Calculation::approximateOutput() {
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
  UserExpression e = UserExpression::Builder(approximatedOutputTree());
  assert(!e.isUninitialized());
  return e;
}

Layout Calculation::createInputLayout(Context* context) {
  ExceptionCheckpoint ecp;
  if (ExceptionRun(ecp)) {
    UserExpression e = input();
    if (!e.isUninitialized()) {
      return e.createLayout(Preferences::PrintFloatMode::Decimal,
                            PrintFloat::k_maxNumberOfSignificantDigits,
                            context);
    }
  }
  return Layout();
}

Layout Calculation::createExactOutputLayout(Context* context,
                                            bool* couldNotCreateExactLayout) {
  ExceptionCheckpoint ecp;
  if (ExceptionRun(ecp)) {
    UserExpression e = exactOutput();
    if (!e.isUninitialized()) {
      return e.createLayout(Preferences::PrintFloatMode::Decimal,
                            PrintFloat::k_maxNumberOfSignificantDigits,
                            context);
    }
  }
  *couldNotCreateExactLayout = true;
  return Layout();
}

Layout Calculation::createApproximateOutputLayout(
    Context* context, bool* couldNotCreateApproximateLayout, bool forEditing) {
  ExceptionCheckpoint ecp;
  if (ExceptionRun(ecp)) {
    UserExpression e = approximateOutput();
    if (!e.isUninitialized()) {
      return e.createLayout(
          m_calculationPreferences.displayMode,
          forEditing ? PrintFloat::k_maxNumberOfSignificantDigits
                     : m_calculationPreferences.numberOfSignificantDigits,
          context);
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

void Calculation::computeDisplayOutput(Poincare::Context* context) {
  if (m_displayOutput != DisplayOutput::Unknown) {
    return;
  }
  m_displayOutput = ComputeDisplayOutput(input(), exactOutput(),
                                         approximateOutput(), context);
  assert(m_displayOutput != DisplayOutput::Unknown);
}

void Calculation::setHeights(KDCoordinate height, KDCoordinate expandedHeight) {
  m_height = height;
  m_expandedHeight = expandedHeight;
}

static bool ShouldOnlyDisplayExactOutput(UserExpression input) {
  /* If the input is a "store in a function", do not display the approximate
   * result. This prevents x->f(x) from displaying x = undef. */
  assert(!input.isUninitialized());
  return input.isStore() && input.cloneChildAtIndex(1).isUserFunction();
}

Calculation::OutputLayouts Calculation::layoutCalculation(
    KDFont::Size font, KDCoordinate maxVisibleWidth, Poincare::Context* context,
    bool canChangeDisplayOutput) {
  /* This processing has do be done in a certain order (1. display output, 2.
   * output layouts, 3. equal sign). */
  computeDisplayOutput(context);
  OutputLayouts outputLayouts = createOutputLayouts(
      context, canChangeDisplayOutput, maxVisibleWidth, font);
  computeEqualSign(outputLayouts, context);
  return outputLayouts;
}

Calculation::OutputLayouts Calculation::createOutputLayouts(
    Context* context, bool canChangeDisplayOutput, KDCoordinate maxVisibleWidth,
    KDFont::Size font) {
  assert(m_displayOutput != DisplayOutput::Unknown);

  // Create the exact output layout
  Layout exactOutput = Layout();
  if (CanDisplayExact(m_displayOutput)) {
    bool couldNotCreateExactLayout = false;
    exactOutput = createExactOutputLayout(context, &couldNotCreateExactLayout);
    if (couldNotCreateExactLayout) {
      if (canChangeDisplayOutput &&
          m_displayOutput != DisplayOutput::ExactOnly) {
        forceDisplayOutput(DisplayOutput::ApproximateOnly);
      } else {
        /* We should only display the exact result, but we cannot create it
         * -> raise an exception. */
        ExceptionCheckpoint::Raise();
      }
    }
    if (canChangeDisplayOutput &&
        m_displayOutput == DisplayOutput::ExactAndApproximate &&
        exactOutput->layoutSize(font).width() > maxVisibleWidth) {
      forceDisplayOutput(DisplayOutput::ExactAndApproximateToggle);
    }
  }

  // Create the approximate output layout
  Layout approximateOutput = Layout();
  if (CanDisplayApproximate(m_displayOutput)) {
    bool couldNotCreateApproximateLayout = false;
    approximateOutput = createApproximateOutputLayout(
        context, &couldNotCreateApproximateLayout);
    if (couldNotCreateApproximateLayout) {
      if (canChangeDisplayOutput &&
          m_displayOutput != DisplayOutput::ApproximateOnly) {
        /* Set display the output to ApproximateOnly, make room in the pool by
         * erasing the exact layout, retry to create the approximate layout. */
        forceDisplayOutput(DisplayOutput::ApproximateOnly);
        exactOutput = Layout();
        couldNotCreateApproximateLayout = false;
        approximateOutput = createApproximateOutputLayout(
            context, &couldNotCreateApproximateLayout);
        if (couldNotCreateApproximateLayout) {
          ExceptionCheckpoint::Raise();
        }
      } else {
        ExceptionCheckpoint::Raise();
      }
    }
  }

  // Hide exact output layout if identical to approximate
  if (CanDisplayExact(m_displayOutput) &&
      CanDisplayApproximate(m_displayOutput) &&
      exactOutput.isIdenticalTo(approximateOutput)) {
    forceDisplayOutput(DisplayOutput::ApproximateIsIdenticalToExact);
  }

  return {exactOutput, approximateOutput};
}

Calculation::DisplayOutput Calculation::ComputeDisplayOutput(
    UserExpression input, UserExpression exactOutput,
    UserExpression approximateOutput, Poincare::Context* context) {
  using enum Calculation::Calculation::DisplayOutput;
  if (input.isUninitialized() || exactOutput.isUninitialized() ||
      ShouldOnlyDisplayExactOutput(input) ||
      exactOutput.isUndefinedOrNonReal()) {
    return ExactOnly;
  }
  if (approximateOutput.isUndefinedOrNonReal() ||
      // Other conditions are factorized in CAS
      CAS::ShouldOnlyDisplayApproximation(input, exactOutput, approximateOutput,
                                          context)) {
    return ApproximateOnly;
  }
  if (input.isIdenticalTo(exactOutput) ||
      input.recursivelyMatches(&NewExpression::isApproximate, context) ||
      exactOutput.recursivelyMatches(&NewExpression::isApproximate, context) ||
      input.recursivelyMatches(&NewExpression::isPercent, context)) {
    return ExactAndApproximateToggle;
  }
  return ExactAndApproximate;
}

Calculation::EqualSign Calculation::ComputeEqualSignFromOutputs(
    const OutputLayouts& outputLayouts,
    Poincare::Internal::ComplexFormat complexFormat,
    Poincare::Internal::AngleUnit angleUnit, Poincare::Context* context) {
  /* Displaying the right equal symbol is less important than displaying a
   * result, so we do not want computeEqualSign to create a pool failure that
   * would prevent from displaying a result that we managed to compute. We thus
   * encapsulate the method in an exception checkpoint: if there was not enough
   * memory on the pool to compute the equal sign, just return
   * EqualSign::Approximation. We can safely use an exception checkpoint here
   * because we are sure of not modifying any pre-existing node in the pool. We
   * are sure there cannot be a Store in the exactOutput. */
  ExceptionCheckpoint ecp;
  if (ExceptionRun(ecp)) {
    /* The output Layouts are converted back to Expressions so that they can be
     * compared */
    assert(!outputLayouts.exact.isUninitialized());
    assert(!outputLayouts.approximate.isUninitialized());
    UserExpression exactDisplayOutput =
        Expression::Parse(outputLayouts.exact.cloneWithoutMargins(), context);
    UserExpression approximateDisplayOutput = Expression::Parse(
        outputLayouts.approximate.cloneWithoutMargins(), context);

    Internal::ProjectionContext ctx{.m_complexFormat = complexFormat,
                                    .m_angleUnit = angleUnit};
    return Poincare::ExactAndApproximateExpressionsAreStrictlyEqual(
               exactDisplayOutput, approximateDisplayOutput, &ctx)
               ? EqualSign::Equal
               : EqualSign::Approximation;
  } else {
    return EqualSign::Approximation;
  }
}

Calculation::EqualSign Calculation::equalSign() const {
  assert(m_equalSign != EqualSign::Unknown);
  return m_equalSign;
}

void Calculation::computeEqualSign(const OutputLayouts& outputLayouts,
                                   Poincare::Context* context) {
  assert(m_displayOutput != DisplayOutput::Unknown);
  if (m_equalSign != EqualSign::Unknown) {
    return;
  }
  if (m_displayOutput == DisplayOutput::ExactOnly ||
      m_displayOutput == DisplayOutput::ApproximateOnly ||
      m_displayOutput == DisplayOutput::ApproximateIsIdenticalToExact) {
    m_equalSign = EqualSign::Undefined;
  } else {
    m_equalSign = ComputeEqualSignFromOutputs(
        outputLayouts, m_calculationPreferences.complexFormat,
        m_calculationPreferences.angleUnit, context);
  }
  assert(m_equalSign != EqualSign::Unknown);
}

void Calculation::fillExpressionsForAdditionalResults(
    UserExpression* input, UserExpression* exactOutput,
    UserExpression* approximateOutput, Context* context) {
  *input = this->input();
  *approximateOutput = this->approximateOutput();
  *exactOutput = m_displayOutput == DisplayOutput::ApproximateOnly
                     ? *approximateOutput
                     : this->exactOutput();
}

AdditionalResultsType Calculation::additionalResultsType(Context* context) {
  if (m_additionalResultsType.isUninitialized()) {
    UserExpression i, a, e;
    fillExpressionsForAdditionalResults(&i, &e, &a, context);
    m_additionalResultsType =
        AdditionalResultsType::AdditionalResultsForExpressions(
            i, e, a, m_calculationPreferences, context);
  }
  assert(!m_additionalResultsType.isUninitialized());
  return m_additionalResultsType;
}

}  // namespace Calculation
