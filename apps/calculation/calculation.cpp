#include "calculation.h"
#include <string.h>

namespace Calculation {

Calculation::Calculation() :
  m_expression(nullptr),
  m_layout(nullptr),
  m_evaluation(0.0f)
{
}

Calculation & Calculation::operator= (const Calculation & other) {
  strlcpy(m_text, other.m_text, sizeof(m_text));
  if (m_expression != nullptr) {
    delete m_expression;
  }
  m_expression = Expression::parse(m_text);
  if (m_layout != nullptr) {
    delete m_layout;
  }
  if (m_expression) {
    m_layout = m_expression->createLayout();
  }
  m_evaluation = other.m_evaluation;
  return *this;
}

void Calculation::setContent(const char * c, Context * context) {
  strlcpy(m_text, c, sizeof(m_text));
  if (m_expression != nullptr) {
    delete m_expression;
  }
  m_expression = Expression::parse(m_text);
  if (m_layout != nullptr) {
    delete m_layout;
  }
  m_layout = expression()->createLayout();
  m_evaluation = m_expression->approximate(*context);
}

Calculation::~Calculation() {
  if (m_layout != nullptr) {
    delete m_layout;
  }
  if (m_expression != nullptr) {
    delete m_expression;
  }
}

const char * Calculation::text() {
  return m_text;
}

Expression * Calculation::expression() {
  return m_expression;
}

ExpressionLayout * Calculation::layout() {
  return m_layout;
}

float Calculation::evaluation() {
  return m_evaluation;
}

bool Calculation::isEmpty() {
  if (m_expression == nullptr) {
    return true;
  }
  return false;
}

}
