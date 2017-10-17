#include "calculation.h"
#include <string.h>
#include <cmath>
using namespace Poincare;

namespace Calculation {

Calculation::Calculation() :
  m_inputText(),
  m_outputText(),
  m_input(nullptr),
  m_inputLayout(nullptr),
  m_output(nullptr),
  m_outputLayout(nullptr)
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
  if (m_output != nullptr) {
    delete m_output;
    m_output = nullptr;
  }
  if (m_outputLayout != nullptr) {
    delete m_outputLayout;
    m_outputLayout = nullptr;
  }
}

Calculation& Calculation::operator=(const Calculation& other) {
  const char * otherInputText = other.m_inputText;
  const char * otherOutputText = other.m_outputText;
  reset();
  strlcpy(m_inputText, otherInputText, sizeof(m_inputText));
  strlcpy(m_outputText, otherOutputText, sizeof(m_outputText));
  return *this;
}

void Calculation::reset() {
  m_inputText[0] = 0;
  m_outputText[0] = 0;
  tidy();
}

void Calculation::setContent(const char * c, Context * context) {
  reset();
  strlcpy(m_inputText, c, sizeof(m_inputText));
  Evaluation<double> * evaluation = input()->evaluate<double>(*context);
  evaluation->writeTextInBuffer(m_outputText, sizeof(m_outputText));
  delete evaluation;
}

const char * Calculation::inputText() {
  return m_inputText;
}

const char * Calculation::outputText() {
  return m_outputText;
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

Evaluation<double> * Calculation::output(Context * context) {
  if (m_output == nullptr) {
    /* To ensure that the expression 'm_output' is a matrix or a complex, we
     * call 'evaluate'. */
    Expression * exp = Expression::parse(m_outputText);
    if (exp != nullptr) {
      m_output = exp->evaluate<double>(*context);
      delete exp;
    } else {
      m_output = new Complex<double>(Complex<double>::Float(NAN));
    }
  }
  return m_output;
}

ExpressionLayout * Calculation::outputLayout(Context * context) {
  if (m_outputLayout == nullptr && output(context) != nullptr) {
    m_outputLayout = output(context)->createLayout();
  }
  return m_outputLayout;
}

bool Calculation::isEmpty() {
  /* To test if a calculation is empty, we need to test either m_inputText or
   * m_outputText, the only two fields that are not lazy-loaded. We choose
   * m_outputText to consider that a calculation being added is still empty
   * until the end of the method 'setContent'. Indeed, during 'setContent'
   * method, 'ans' evaluation calls the evaluation of the last calculation
   * only if the calculation being filled is not taken into account.*/
  if (strlen(m_outputText) == 0) {
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
  if (m_output != nullptr) {
    delete m_output;
  }
  m_output = nullptr;
  if (m_outputLayout != nullptr) {
    delete m_outputLayout;
  }
  m_outputLayout = nullptr;
}

}
