#include "calculation.h"
#include <string.h>
using namespace Poincare;

namespace Calculation {

Calculation::Calculation() :
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

void Calculation::reset() {
  m_inputText[0] = 0;
  m_outputText[0] = 0;
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

void Calculation::setContent(const char * c, Context * context) {
  strlcpy(m_inputText, c, sizeof(m_inputText));
  if (m_input != nullptr) {
    delete m_input;
  }
  m_input = Expression::parse(m_inputText);
  if (m_inputLayout != nullptr) {
    delete m_inputLayout;
  }
  m_inputLayout = m_input->createLayout(Expression::FloatDisplayMode::Decimal, Expression::ComplexFormat::Algebric);
  if (m_output != nullptr) {
    delete m_output;
  }
  m_output = m_input->evaluate(*context);
  if (m_outputLayout != nullptr) {
    delete m_outputLayout;
  }
  m_outputLayout = m_output->createLayout();
  m_output->writeTextInBuffer(m_outputText, sizeof(m_outputText));
}

const char * Calculation::inputText() {
  return m_inputText;
}

const char * Calculation::outputText() {
  return m_outputText;
}

Expression * Calculation::input() {
  return m_input;
}

ExpressionLayout * Calculation::inputLayout() {
  return m_inputLayout;
}

Expression * Calculation::output() {
  return m_output;
}

ExpressionLayout * Calculation::outputLayout() {
  return m_outputLayout;
}

bool Calculation::isEmpty() {
  if (m_output == nullptr) {
    return true;
  }
  return false;
}

}
