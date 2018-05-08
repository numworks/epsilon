#include "calculation.h"
#include "calculation_store.h"
#include <string.h>
#include <cmath>
using namespace Poincare;

namespace Calculation {

Calculation::Calculation() :
  m_inputText(),
  m_exactOutputText(),
  m_approximateOutputText(),
  m_input(nullptr),
  m_inputLayout(nullptr),
  m_exactOutput(nullptr),
  m_exactOutputLayout(nullptr),
  m_approximateOutput(nullptr),
  m_approximateOutputLayout(nullptr)
{
}

Calculation::~Calculation() {
  if (m_inputLayout != nullptr) {
    delete m_inputLayout;
    m_inputLayout = nullptr;
  }
  if (m_input != nullptr) {
    delete m_input;
    m_input = nullptr;
  }
  if (m_exactOutput != nullptr) {
    delete m_exactOutput;
    m_exactOutput = nullptr;
  }
  if (m_exactOutputLayout != nullptr) {
    delete m_exactOutputLayout;
    m_exactOutputLayout = nullptr;
  }
  if (m_approximateOutput != nullptr) {
    delete m_approximateOutput;
    m_approximateOutput = nullptr;
  }
  if (m_approximateOutputLayout != nullptr) {
    delete m_approximateOutputLayout;
    m_approximateOutputLayout = nullptr;
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
  /* We do not store directly the text enter by the user but its serialization
   * to be able to compare it to the exact ouput text. */
  m_input->writeTextInBuffer(m_inputText, sizeof(m_inputText));
  m_exactOutput = Expression::ParseAndSimplify(m_inputText, *context);
  m_exactOutput->writeTextInBuffer(m_exactOutputText, sizeof(m_exactOutputText));
  m_approximateOutput = m_exactOutput->approximate<double>(*context);
  m_approximateOutput->writeTextInBuffer(m_approximateOutputText, sizeof(m_approximateOutputText));
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

ExpressionLayout * Calculation::inputLayout() {
  if (m_inputLayout == nullptr && input() != nullptr) {
    m_inputLayout = input()->createLayout(PrintFloat::Mode::Decimal, Expression::ComplexFormat::Cartesian);
  }
  return m_inputLayout;
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
  if (m_inputLayout != nullptr) {
    delete m_inputLayout;
  }
  m_inputLayout = nullptr;
  if (m_exactOutput != nullptr) {
    delete m_exactOutput;
  }
  m_exactOutput = nullptr;
  if (m_exactOutputLayout != nullptr) {
    delete m_exactOutputLayout;
  }
  m_exactOutputLayout = nullptr;
  if (m_approximateOutput != nullptr) {
    delete m_approximateOutput;
  }
  m_approximateOutput = nullptr;
  if (m_approximateOutputLayout != nullptr) {
    delete m_approximateOutputLayout;
  }
  m_approximateOutputLayout = nullptr;
}

Expression * Calculation::exactOutput(Context * context) {
  if (m_exactOutput == nullptr) {
    /* To ensure that the expression 'm_exactOutput' is a simplified, we
     * call 'ParseAndSimplify'. */
    m_exactOutput = Expression::ParseAndSimplify(m_exactOutputText, *context);
  }
  return m_exactOutput;
}

ExpressionLayout * Calculation::exactOutputLayout(Context * context) {
  if (m_exactOutputLayout == nullptr && exactOutput(context) != nullptr) {
    m_exactOutputLayout = exactOutput(context)->createLayout();
  }
  return m_exactOutputLayout;
}

Expression * Calculation::approximateOutput(Context * context) {
  if (m_approximateOutput == nullptr) {
    /* To ensure that the expression 'm_output' is a matrix or a complex, we
     * call 'evaluate'. */
    Expression * exp = Expression::parse(m_approximateOutputText);
    if (exp != nullptr) {
      m_approximateOutput = exp->approximate<double>(*context);
      delete exp;
    } else {
      m_approximateOutput = new Complex<double>(Complex<double>::Float(NAN));
    }
  }
  return m_approximateOutput;
}

ExpressionLayout * Calculation::approximateOutputLayout(Context * context) {
  if (m_approximateOutputLayout == nullptr && approximateOutput(context) != nullptr) {
    m_approximateOutputLayout = approximateOutput(context)->createLayout();
  }
  return m_approximateOutputLayout;
}

bool Calculation::shouldDisplayApproximateOutput(Context * context) {
  if (strcmp(m_exactOutputText, m_approximateOutputText) == 0) {
    return true;
  }
  if (strcmp(m_exactOutputText, m_inputText) == 0) {
    return true;
  }
  return input()->isApproximate(*context);
}

bool Calculation::exactAndApproximateDisplayedOutputsAreEqual(Poincare::Context * context) {
  char buffer[k_printedExpressionSize];
  approximateOutput(context)->writeTextInBuffer(buffer, k_printedExpressionSize, Preferences::sharedPreferences()->numberOfSignificantDigits());
  /* Warning: we cannot use directly the m_approximateOutputText but we have to
   * re-serialize the approximateOutput because the number of stored
   * significative numbers and the number of displayed significative numbers
   * are not identical. (For example, 0.000025 might be displayed "0.00003"
   * which requires in an approximative equal) */
  Expression * approximateOutput = Expression::ParseAndSimplify(buffer, *context);
  bool isEqual = approximateOutput->isIdenticalTo(exactOutput(context));
  delete approximateOutput;
  return isEqual;
}

}
