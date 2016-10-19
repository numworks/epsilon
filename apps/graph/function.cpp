#include "function.h"
#include <string.h>

namespace Graph {

Function::Function(const char * text, KDColor color) :
  m_name(text),
  m_color(color),
  m_expression(nullptr),
  m_layout(nullptr),
  m_active(true),
  m_displayDerivative(false)
{
}

void Function::setContent(const char * c) {
  strlcpy(m_text, c, sizeof(m_text));
  if (m_expression != nullptr) {
    delete m_expression;
  }
  m_expression = Expression::parse(m_text);
  if (m_layout != nullptr) {
    delete m_layout;
  }
  m_layout = expression()->createLayout();
}

void Function::setColor(KDColor color) {
  m_color = color;
}

Function::~Function() {
  if (m_layout != nullptr) {
    delete m_layout;
  }
  if (m_expression != nullptr) {
    delete m_expression;
  }
}

const char * Function::text() {
  return m_text;
}

const char * Function::name() {
  return m_name;
}

Expression * Function::expression() {
  return m_expression;
}

ExpressionLayout * Function::layout() {
  return m_layout;
}

bool Function::isActive() {
  return m_active;
}

bool Function::displayDerivative() {
  return m_displayDerivative;
}

void Function::setActive(bool active) {
  m_active = active;
}

void Function::setDisplayDerivative(bool display) {
  m_displayDerivative = display;
}

float Function::evaluateAtAbscissa(float x, EvaluateContext * context) const {
  context->setOverridenValueForSymbolX(x);
  return m_expression->approximate(*context);
}

float Function::approximateDerivative(float x, EvaluateContext * context) const {
  float functionPlus = evaluateAtAbscissa(x + k_epsilon, context);
  float functionMinus = evaluateAtAbscissa(x - k_epsilon, context);
  float growthRate = (functionPlus - functionMinus)/(2*k_epsilon);
  return growthRate;
}

}
