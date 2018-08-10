#include "calculation.h"
#include "calculation_store.h"
#include "../shared/poincare_helpers.h"
#include <string.h>
#include <cmath>

using namespace Poincare;
using namespace Shared;

namespace Calculation {

Calculation::Calculation() :
  m_inputText(),
  m_exactOutputText(),
  m_approximateOutputText(),
  m_input(nullptr),
  m_exactOutput(nullptr),
  m_approximateOutput(nullptr),
  m_height(-1),
  m_equalSign(EqualSign::Unknown)
{
}

Calculation::~Calculation() {
  if (m_input != nullptr) {
    delete m_input;
    m_input = nullptr;
  }
  if (m_exactOutput != nullptr) {
    delete m_exactOutput;
    m_exactOutput = nullptr;
  }
  if (m_approximateOutput != nullptr) {
    delete m_approximateOutput;
    m_approximateOutput = nullptr;
  }
}

Calculation& Calculation::operator=(const Calculation& other) {
  const char * otherInputText = other.m_inputText;
  const char * otherExactOutputText = other.m_exactOutputText;
  const char * otherApproximateOutputText = other.m_approximateOutputText;
  reset();
  strlcpy(m_inputText, otherInputText, sizeof(m_inputText));
  strlcpy(m_exactOutputText, otherExactOutputText, sizeof(m_exactOutputText));
  strlcpy(m_approximateOutputText, otherApproximateOutputText, sizeof(m_approximateOutputText));
  return *this;
}

void Calculation::reset() {
  m_inputText[0] = 0;
  m_exactOutputText[0] = 0;
  m_approximateOutputText[0] = 0;
  tidy();
}

void Calculation::setContent(const char * c, Context * context, Expression * ansExpression) {
  reset();
  m_input = Expression::parse(c);
  Expression::ReplaceSymbolWithExpression(&m_input, Symbol::SpecialSymbols::Ans, ansExpression);
  /* We do not store directly the text enter by the user because we do not want
   * to keep Ans symbol in the calculation store. */
  PoincareHelpers::Serialize(m_input, m_inputText, sizeof(m_inputText));
  m_exactOutput = PoincareHelpers::ParseAndSimplify(m_inputText, *context);
  PoincareHelpers::Serialize(m_exactOutput, m_exactOutputText, sizeof(m_exactOutputText));
  m_approximateOutput = PoincareHelpers::Approximate<double>(m_exactOutput, *context);
  PoincareHelpers::Serialize(m_approximateOutput, m_approximateOutputText, sizeof(m_approximateOutputText));
}

KDCoordinate Calculation::height(Context * context) {
  if (m_height < 0) {
    LayoutRef inputLayout = createInputLayout();
    if (inputLayout.isAllocationFailure()) {
      /* If there is not enough tree pool space to create the layout, return a
       * default height. Do not store it in m_height in case some memory gets
       * freed afterwards. */
      return k_heightComputationFailureHeight;
    }
    KDCoordinate inputHeight = inputLayout.layoutSize().height();
    LayoutRef approximateLayout = createApproximateOutputLayout(context);
    if (approximateLayout.isAllocationFailure()) {
      /* If there is not enough tree pool space to create the layout, return a
       * default height. Do not store it in m_height in case some memory gets
       * freed afterwards. */
      return k_heightComputationFailureHeight;
    }
    KDCoordinate approximateOutputHeight = approximateLayout.layoutSize().height();
    if (shouldOnlyDisplayApproximateOutput(context)) {
      m_height = inputHeight+approximateOutputHeight;
    } else {
      LayoutRef exactLayout = createExactOutputLayout(context);
      if (exactLayout.isAllocationFailure()) {
        /* If there is not enough tree pool space to create the layout, return a
         * default height. Do not store it in m_height in case some memory gets
         * freed afterwards. */
        return k_heightComputationFailureHeight;
      }
      KDCoordinate exactOutputHeight = exactLayout.layoutSize().height();
      KDCoordinate outputHeight = max(exactLayout.baseline(), approximateLayout.baseline()) + max(exactOutputHeight-exactLayout.baseline(), approximateOutputHeight-approximateLayout.baseline());
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

Expression * Calculation::input() {
  if (m_input == nullptr) {
    m_input = Expression::parse(m_inputText);
  }
  return m_input;
}

LayoutRef Calculation::createInputLayout() {
  if (input() != nullptr) {
    return input()->createLayout(Preferences::PrintFloatMode::Decimal, PrintFloat::k_numberOfStoredSignificantDigits);
  }
  return LayoutRef(nullptr);
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
  if (m_input != nullptr) {
    delete m_input;
  }
  m_input = nullptr;
  if (m_exactOutput != nullptr) {
    delete m_exactOutput;
  }
  m_exactOutput = nullptr;
  if (m_approximateOutput != nullptr) {
    delete m_approximateOutput;
  }
  m_approximateOutput = nullptr;
  m_height = -1;
  m_equalSign = EqualSign::Unknown;
}

Expression * Calculation::exactOutput(Context * context) {
  if (m_exactOutput == nullptr) {
    /* Because the angle unit might have changed, we do not simplify again. We
     * thereby avoid turning cos(Pi/4) into sqrt(2)/2 and displaying
     * 'sqrt(2)/2 = 0.999906' (which is totally wrong) instead of
     * 'cos(pi/4) = 0.999906' (which is true in degree). */
    m_exactOutput = Expression::parse(m_exactOutputText);
    if (m_exactOutput == nullptr) {
      m_exactOutput = new Undefined();
    }
  }
  return m_exactOutput;
}

LayoutRef Calculation::createExactOutputLayout(Context * context) {
  if (exactOutput(context) != nullptr) {
    return PoincareHelpers::CreateLayout(exactOutput(context));
  }
  return LayoutRef(nullptr);
}

Expression * Calculation::approximateOutput(Context * context) {
  if (m_approximateOutput == nullptr) {
    /* To ensure that the expression 'm_output' is a matrix or a complex, we
     * call 'evaluate'. */
    Expression * exp = Expression::parse(m_approximateOutputText);
    if (exp != nullptr) {
      m_approximateOutput = PoincareHelpers::Approximate<double>(exp, *context);
      delete exp;
    } else {
      m_approximateOutput = new Undefined();
    }
  }
  return m_approximateOutput;
}

LayoutRef Calculation::createApproximateOutputLayout(Context * context) {
  if (approximateOutput(context) != nullptr) {
    return PoincareHelpers::CreateLayout(approximateOutput(context));
  }
  return LayoutRef(nullptr);
}

bool Calculation::shouldOnlyDisplayApproximateOutput(Context * context) {
  if (strcmp(m_exactOutputText, m_approximateOutputText) == 0) {
    return true;
  }
  if (strcmp(m_exactOutputText, "undef") == 0) {
    return true;
  }
  return input()->isApproximate(*context);
}

Calculation::EqualSign Calculation::exactAndApproximateDisplayedOutputsAreEqual(Poincare::Context * context) {
  if (m_equalSign != EqualSign::Unknown) {
    return m_equalSign;
  }
  m_equalSign = exactOutput(context)->isEqualToItsApproximationLayout(approximateOutput(context), k_printedExpressionSize, Preferences::sharedPreferences()->angleUnit(), Preferences::sharedPreferences()->displayMode(), Preferences::sharedPreferences()->numberOfSignificantDigits(), *context) ? EqualSign::Equal : EqualSign::Approximation;
  return m_equalSign;
}

}
