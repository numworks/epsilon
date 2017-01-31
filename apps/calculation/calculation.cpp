#include "calculation.h"
#include <string.h>

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
  }
  if (m_input != nullptr) {
    delete m_input;
  }
  if (m_output != nullptr) {
    delete m_output;
  }
  if (m_outputLayout != nullptr) {
    delete m_outputLayout;
  }
}

void Calculation::reset() {
  m_text[0] = 0;
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

void Calculation::setContent(const char * c, Context * context, Preferences * preferences) {
  strlcpy(m_text, c, sizeof(m_text));
  if (m_input != nullptr) {
    delete m_input;
  }
  m_input = Expression::parse(m_text);
  if (m_inputLayout != nullptr) {
    delete m_inputLayout;
  }
  m_inputLayout = m_input->createLayout(preferences->displayMode());
  if (m_output != nullptr) {
    delete m_output;
  }
  m_output = m_input->evaluate(*context);
  if (m_outputLayout != nullptr) {
    delete m_outputLayout;
  }
  m_outputLayout = m_output->createLayout(preferences->displayMode());
}

const char * Calculation::text() {
  return m_text;
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
