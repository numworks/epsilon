#include "calculation.h"
#include <string.h>

namespace Calculation {

Calculation::Calculation() :
  m_text(""),
  m_expression(nullptr),
  m_layout(nullptr),
  m_evaluation(Float(0.0f))
{
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

Float * Calculation::evaluation() {
  return &m_evaluation;
}

}
