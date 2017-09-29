#include "calculation.h"
#include <string.h>
#include <math.h>
using namespace Poincare;

namespace Calculation {

Calculation::Calculation() :
  m_inputText(""),
  m_exactOutputText(""),
  m_approximateOutputText(""),
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

void Calculation::setContent(const char * c, Context * context) {
  reset();
  strlcpy(m_inputText, c, sizeof(m_inputText));
  m_exactOutput = input()->clone();
  Expression::Simplify(&m_exactOutput, *context);
  m_exactOutput->writeTextInBuffer(m_exactOutputText, sizeof(m_exactOutputText));
  m_approximateOutput = m_exactOutput->evaluate<double>(*context);
  m_approximateOutput->writeTextInBuffer(m_approximateOutputText, sizeof(m_approximateOutputText));
}

const char * Calculation::inputText() {
  return m_inputText;
}

const char * Calculation::outputText() {
  if (shouldApproximateOutput()) {
    return m_approximateOutputText;
  }
  return m_exactOutputText;
}

Expression * Calculation::input() {
  if (m_input == nullptr) {
    m_input = Expression::parse(m_inputText);
  }
  return m_input;
}

ExpressionLayout * Calculation::inputLayout() {
  if (m_inputLayout == nullptr && input() != nullptr) {
    m_inputLayout = input()->createLayout(Expression::FloatDisplayMode::Decimal, Expression::ComplexFormat::Cartesian);
  }
  return m_inputLayout;
}

Expression * Calculation::output(Context * context) {
  if (shouldApproximateOutput()) {
    return approximateOutput(context);
  }
  return exactOutput(context);
}

ExpressionLayout * Calculation::outputLayout(Context * context) {
  if (shouldApproximateOutput()) {
    return approximateOutputLayout(context);
  }
  return exactOutputLayout(context);
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
     * call 'simplifyAndBeautify'. */
    m_exactOutput = Expression::parse(m_exactOutputText);
    if (m_exactOutput != nullptr) {
      Expression::Simplify(&m_exactOutput, *context);
    } else {
      m_exactOutput = new Undefined();
    }
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
      m_approximateOutput = exp->evaluate<double>(*context);
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

bool Calculation::shouldApproximateOutput() {
  return input()->recursivelyMatches([](const Expression * e) {
        return e->type() == Expression::Type::Decimal || Expression::IsMatrix(e);
      });
}

}
