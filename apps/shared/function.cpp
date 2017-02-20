#include "function.h"
#include <string.h>
#include <math.h>

using namespace Poincare;

namespace Shared {

Function::Function(const char * text, KDColor color) :
  m_expression(nullptr),
  m_text{0},
  m_name(text),
  m_color(color),
  m_layout(nullptr),
  m_active(true)
{
}

void Function::setContent(const char * c) {
  strlcpy(m_text, c, sizeof(m_text));
  if (m_expression != nullptr) {
    delete m_expression;
  }
  m_expression = Poincare::Expression::parse(m_text);
  if (m_layout != nullptr) {
    delete m_layout;
  }
  m_layout = nullptr;
  if (m_expression) {
    m_layout = expression()->createLayout(Expression::FloatDisplayMode::Decimal);
  }
}

void Function::setColor(KDColor color) {
  m_color = color;
}

Function::~Function() {
  if (m_layout != nullptr) {
    delete m_layout;
    m_layout = nullptr;
  }
  if (m_expression != nullptr) {
    delete m_expression;
    m_expression = nullptr;
  }
}

const char * Function::text() {
  return m_text;
}

const char * Function::name() {
  return m_name;
}

Poincare::Expression * Function::expression() {
  return m_expression;
}

Poincare::ExpressionLayout * Function::layout() {
  return m_layout;
}

bool Function::isActive() {
  return m_active;
}

void Function::setActive(bool active) {
  m_active = active;
}

float Function::evaluateAtAbscissa(float x, Poincare::Context * context) const {
  Poincare::Symbol xSymbol = Poincare::Symbol(symbol());
  Poincare::Complex e = Poincare::Complex::Float(x);
  context->setExpressionForSymbolName(&e, &xSymbol);
  return m_expression->approximate(*context);
}

}
