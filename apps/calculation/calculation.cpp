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

Calculation & Calculation::operator= (const Calculation & other) {
  strlcpy(m_text, other.m_text, sizeof(m_text));
  if (m_input != nullptr) {
    delete m_input;
  }
  m_input = nullptr;
  if (other.m_input) {
    m_input = Expression::parse(m_text);
  }
  if (m_inputLayout != nullptr) {
    delete m_inputLayout;
  }
  m_inputLayout = nullptr;
  if (m_input && other.m_inputLayout) {
    m_inputLayout = m_input->createLayout();
  }
  if (m_output != nullptr) {
    delete m_output;
  }
  m_output = nullptr;
  if (other.m_output) {
    m_output = other.m_output->clone();
  }
  if (m_outputLayout != nullptr) {
    delete m_outputLayout;
  }
  m_outputLayout = nullptr;
  if (m_output && other.m_outputLayout) {
    m_outputLayout = m_output->createLayout();
  }
  return *this;
}

void Calculation::setContent(const char * c, Context * context) {
  strlcpy(m_text, c, sizeof(m_text));
  if (m_input != nullptr) {
    delete m_input;
  }
  m_input = Expression::parse(m_text);
  if (m_inputLayout != nullptr) {
    delete m_inputLayout;
  }
  m_inputLayout = m_input->createLayout();
  if (m_output != nullptr) {
    delete m_outputLayout;
  }
  m_output = m_input->evaluate(*context);
  if (m_outputLayout != nullptr) {
    delete m_outputLayout;
  }
  m_outputLayout = m_output->createLayout();
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
  if (m_input == nullptr) {
    return true;
  }
  return false;
}

}
