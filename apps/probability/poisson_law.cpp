#include "poisson_law.h"
#include <assert.h>
#include <math.h>

namespace Probability {

PoissonLaw::PoissonLaw(EvaluateContext * evaluateContext) :
  OneParameterLaw(evaluateContext),
  m_expression(Expression::parse("p1*t"))
{
  //m_expression = Expression::parse("exp(-p1)*p1^t/t!");
  assert(m_expression != nullptr);
}

PoissonLaw::~PoissonLaw() {
  delete m_expression;
}

const char * PoissonLaw::title() {
  return "Loi Poisson";
}

Expression * PoissonLaw::expression() const {
  return m_expression;
}

Law::Type PoissonLaw::type() const {
  return Type::Poisson;
}

bool PoissonLaw::isContinuous() {
  return false;
}

const char * PoissonLaw::parameterNameAtIndex(int index) {
  assert(index == 0);
  return "l";
}

const char * PoissonLaw::parameterDefinitionAtIndex(int index) {
  assert(index == 0);
  return "l : parametre";
}

float PoissonLaw::xMin() {
  return 0.0f;
}

float PoissonLaw::xMax() {
  if (m_parameter1 == 0.0f) {
    return 1.0f;
  }
  return m_parameter1 + 5.0f*sqrtf(m_parameter1);
}

float PoissonLaw::yMin() {
  return -0.2f;
}

float PoissonLaw::yMax() {
  return 1.0f;
}

}
