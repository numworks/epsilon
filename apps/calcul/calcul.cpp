#include "calcul.h"
#include <string.h>

namespace Calcul {

Calcul::Calcul() :
  m_text(""),
  m_expression(nullptr),
  m_layout(nullptr),
  m_evaluation(Float(0.0f))
{
}

void Calcul::setContent(const char * c, Context * context) {
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

Calcul::~Calcul() {
  if (m_layout != nullptr) {
    delete m_layout;
  }
  if (m_expression != nullptr) {
    delete m_expression;
  }
}

const char * Calcul::text() {
  return m_text;
}

Expression * Calcul::expression() {
  return m_expression;
}

ExpressionLayout * Calcul::layout() {
  return m_layout;
}

Float * Calcul::evaluation() {
  return &m_evaluation;
}

}
