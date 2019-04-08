#include "calculation.h"
#include "calculation_store.h"
#include "../shared/poincare_helpers.h"
#include <poincare/symbol.h>
#include <poincare/undefined.h>
#include <poincare/unreal.h>
#include <string.h>
#include <cmath>

using namespace Poincare;
using namespace Shared;

namespace Calculation {

static inline KDCoordinate maxCoordinate(KDCoordinate x, KDCoordinate y) { return x > y ? x : y; }

Calculation::Calculation() :
  m_inputText(),
  m_exactOutputText(),
  m_approximateOutputText(),
  m_height(-1),
  m_equalSign(EqualSign::Unknown)
{
}

bool Calculation::operator==(const Calculation& c) {
  return strcmp(m_inputText, c.m_inputText) == 0
      && strcmp(m_approximateOutputText, c.m_approximateOutputText) == 0
      /* Some calculations can make appear trigonometric functions in their
       * exact output. Their argument will be different with the angle unit
       * preferences but both input and approximate output will be the same.
       * For example, i^(sqrt(3)) = cos(sqrt(3)*pi/2)+i*sin(sqrt(3)*pi/2) if
       * angle unit is radian and i^(sqrt(3)) = cos(sqrt(3)*90+i*sin(sqrt(3)*90)
       * in degree. */
      && strcmp(m_exactOutputText, c.m_exactOutputText) == 0;
}

void Calculation::reset() {
  m_inputText[0] = 0;
  m_exactOutputText[0] = 0;
  m_approximateOutputText[0] = 0;
  tidy();
}

void Calculation::setContent(const char * c, Context * context, Expression ansExpression) {
  reset();
  {
    Symbol ansSymbol = Symbol::Ans();
    Expression input = Expression::Parse(c).replaceSymbolWithExpression(ansSymbol, ansExpression);
    /* We do not store directly the text enter by the user because we do not want
     * to keep Ans symbol in the calculation store. */
    PoincareHelpers::Serialize(input, m_inputText, sizeof(m_inputText));
  }
  Expression exactOutput;
  Expression approximateOutput;
  PoincareHelpers::ParseAndSimplifyAndApproximate(m_inputText, &exactOutput, &approximateOutput, *context, false);
  PoincareHelpers::Serialize(exactOutput, m_exactOutputText, sizeof(m_exactOutputText));
  PoincareHelpers::Serialize(approximateOutput, m_approximateOutputText, sizeof(m_approximateOutputText));
}

KDCoordinate Calculation::height(Context * context) {
  if (m_height < 0) {
    Layout inputLayout = createInputLayout();
    KDCoordinate inputHeight = inputLayout.layoutSize().height();
    Layout approximateLayout = createApproximateOutputLayout(context);
    Layout exactLayout = createExactOutputLayout();
    DisplayOutput display = displayOutput(context);
    if (display == DisplayOutput::ExactOnly) {
      KDCoordinate exactOutputHeight = exactLayout.layoutSize().height();
      m_height = inputHeight+exactOutputHeight;
    } else if (display == DisplayOutput::ApproximateOnly) {
      KDCoordinate approximateOutputHeight = approximateLayout.layoutSize().height();
      m_height = inputHeight+approximateOutputHeight;
    } else {
      assert(display == DisplayOutput::ExactAndApproximate);
      KDCoordinate approximateOutputHeight = approximateLayout.layoutSize().height();
      KDCoordinate exactOutputHeight = exactLayout.layoutSize().height();
      KDCoordinate outputHeight = maxCoordinate(exactLayout.baseline(), approximateLayout.baseline()) + maxCoordinate(exactOutputHeight-exactLayout.baseline(), approximateOutputHeight-approximateLayout.baseline());
      m_height = inputHeight + outputHeight;
    }
  }
  return m_height;
}

const char * Calculation::inputText() {
  return m_inputText;
}

const char * Calculation::exactOutputText() {
  return m_exactOutputText;
}

const char * Calculation::approximateOutputText() {
  return m_approximateOutputText;
}

Expression Calculation::input() {
  return Expression::Parse(m_inputText);
}

Layout Calculation::createInputLayout() {
  return input().createLayout(Preferences::PrintFloatMode::Decimal, PrintFloat::k_numberOfStoredSignificantDigits);
}

bool Calculation::isEmpty() {
  /* To test if a calculation is empty, we need to test either m_inputText or
   * m_exactOutputText or m_approximateOutputText, the only three fields that
   * are not lazy-loaded. We choose m_exactOutputText to consider that a
   * calculation being added is still empty until the end of the method
   * 'setContent'. Indeed, during 'setContent' method, 'ans' evaluation calls
   * the evaluation of the last calculation only if the calculation being
   * filled is not taken into account.*/
  if (strlen(m_approximateOutputText) == 0) {
    return true;
  }
  return false;
}

void Calculation::tidy() {
  /* Uninitialized all Expression stored to free the Pool */
  m_height = -1;
  m_equalSign = EqualSign::Unknown;
}

Expression Calculation::exactOutput() {
  /* Because the angle unit might have changed, we do not simplify again. We
   * thereby avoid turning cos(Pi/4) into sqrt(2)/2 and displaying
   * 'sqrt(2)/2 = 0.999906' (which is totally wrong) instead of
   * 'cos(pi/4) = 0.999906' (which is true in degree). */
  Expression exactOutput = Expression::Parse(m_exactOutputText);
  if (exactOutput.isUninitialized()) {
    return Undefined::Builder();
  }
  return exactOutput;
}

Layout Calculation::createExactOutputLayout() {
  return PoincareHelpers::CreateLayout(exactOutput());
}

Expression Calculation::approximateOutput(Context * context) {
  /* To ensure that the expression 'm_output' is a matrix or a complex, we
   * call 'evaluate'. */
  Expression exp = Expression::Parse(m_approximateOutputText);
  if (exp.isUninitialized()) {
    /* TODO: exp might be uninitialized because the serialization did not fit in
     * the buffer. Put a special error instead of "undef". */
    return Undefined::Builder();
  }
  return PoincareHelpers::Approximate<double>(exp, *context);
}

Layout Calculation::createApproximateOutputLayout(Context * context) {
  return PoincareHelpers::CreateLayout(approximateOutput(context));
}

Calculation::DisplayOutput Calculation::displayOutput(Context * context) {
  if (shouldOnlyDisplayExactOutput()) {
    return DisplayOutput::ExactOnly;
  }
  bool approximateOnly = false;
  if (strcmp(m_exactOutputText, m_approximateOutputText) == 0) {
    /* If the exact and approximate results' texts are equal and their layouts
     * too, do not display the exact result. If the two layouts are not equal
     * because of the number of significant digits, we display both. */
    approximateOnly = exactAndApproximateDisplayedOutputsAreEqual(context) == Calculation::EqualSign::Equal;
  } else if (strcmp(m_exactOutputText, Undefined::Name()) == 0 || strcmp(m_approximateOutputText, Unreal::Name()) == 0) {
    // If the approximate result is 'unreal' or the exact result is 'undef'
    approximateOnly = true;
  } else {
    approximateOnly = input().isApproximate(*context) || exactOutput().isApproximate(*context);
  }
  return approximateOnly ? DisplayOutput::ApproximateOnly : DisplayOutput::ExactAndApproximate;
}

bool Calculation::shouldOnlyDisplayExactOutput() {
  /* If the approximateOutput is undef, do not display it. This prevents:
   * x->f(x) from displaying x = undef
   * x+x from displaying 2x = undef */
  return strcmp(m_approximateOutputText, Undefined::Name()) == 0;
}

Calculation::EqualSign Calculation::exactAndApproximateDisplayedOutputsAreEqual(Poincare::Context * context) {
  if (m_equalSign != EqualSign::Unknown) {
    return m_equalSign;
  }
  constexpr int bufferSize = Constant::MaxSerializedExpressionSize;
  char buffer[bufferSize];
  Preferences * preferences = Preferences::sharedPreferences();
  Expression exactOutputExpression = PoincareHelpers::ParseAndSimplify(m_exactOutputText, *context, false);
  if (exactOutputExpression.isUninitialized()) {
    exactOutputExpression = Undefined::Builder();
  }
  Preferences::ComplexFormat complexFormat = Expression::UpdatedComplexFormatWithTextInput(preferences->complexFormat(), m_inputText);
  m_equalSign = exactOutputExpression.isEqualToItsApproximationLayout(approximateOutput(context), buffer, bufferSize, complexFormat, preferences->angleUnit(), preferences->displayMode(), preferences->numberOfSignificantDigits(), *context) ? EqualSign::Equal : EqualSign::Approximation;
  return m_equalSign;
}

}
