#include "function.h"
#include <string.h>
#include <math.h>

namespace Graph {

Function::Function(const char * text, KDColor color) :
  m_text{0},
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
  m_layout = nullptr;
  if (m_expression) {
    m_layout = expression()->createLayout();
  }
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

float Function::evaluateAtAbscissa(float x, Context * context, Expression::AngleUnit angleUnit) const {
  Symbol xSymbol = Symbol('x');
  Complex e = Complex(x);
  context->setExpressionForSymbolName(&e, &xSymbol);
  return m_expression->approximate(*context, angleUnit);
}

float Function::approximateDerivative(float x, Context * context, Expression::AngleUnit angleUnit) const {
  Complex abscissa = Complex(x);
  Expression * args[2] = {m_expression, &abscissa};
  Derivative derivative = Derivative();
  derivative.setArgument(args, 2, true);
  return derivative.approximate(*context, angleUnit);
}

}
