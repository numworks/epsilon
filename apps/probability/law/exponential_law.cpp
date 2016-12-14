#include "exponential_law.h"
#include <assert.h>

namespace Probability {

ExponentialLaw::ExponentialLaw(EvaluateContext * evaluateContext) :
  OneParameterLaw(evaluateContext),
  m_expression(Expression::parse("p1*t"))
{
  //m_expression = Expression::parse("p1*exp(-p1*t)");
  assert(m_expression != nullptr);
}

ExponentialLaw::~ExponentialLaw() {
  delete m_expression;
}

const char * ExponentialLaw::title() {
  return "Loi exponentielle";
}

Expression * ExponentialLaw::expression() const {
  return m_expression;
}

Law::Type ExponentialLaw::type() const {
  return Type::Exponential;
}

bool ExponentialLaw::isContinuous() {
  return true;
}

const char * ExponentialLaw::parameterNameAtIndex(int index) {
  assert(index == 0);
  return "l";
}

const char * ExponentialLaw::parameterDefinitionAtIndex(int index) {
  assert(index == 0);
  return "l : parametre";
}

float ExponentialLaw::xMin() {
  return 0.0f;
}

float ExponentialLaw::xMax() {
  if (m_parameter1 == 0.0f) {
    return 100.0f;
  }
  return 5.0f/m_parameter1;
}

float ExponentialLaw::yMin() {
  return -0.2f;
}

float ExponentialLaw::yMax() {
  return 1.0f;
}

}
