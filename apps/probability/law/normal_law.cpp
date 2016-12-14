#include "normal_law.h"
#include <assert.h>

namespace Probability {

NormalLaw::NormalLaw(Context * context) :
  TwoParameterLaw(context),
  m_expression(Expression::parse("a-b*t"))
{
  //m_expression = Expression::parse("(1/(b*sqrt(2*Pi))*exp(-0.5*((t-a)/b)^2)");
  assert(m_expression != nullptr);
}

NormalLaw::~NormalLaw() {
  delete m_expression;
}

const char * NormalLaw::title() {
  return "Loi normale";
}

Expression * NormalLaw::expression() const {
  return m_expression;
}

Law::Type NormalLaw::type() const {
  return Type::Normal;
}

bool NormalLaw::isContinuous() {
  return true;
}

const char * NormalLaw::parameterNameAtIndex(int index) {
  assert(index >= 0 && index < 2);
  if (index == 0) {
    return "u";
  } else {
    return "o";
  }
}

const char * NormalLaw::parameterDefinitionAtIndex(int index) {
  assert(index >= 0 && index < 2);
  if (index == 0) {
    return "u : moyenne";
  } else {
    return "o : ecart-type";
  }
}

float NormalLaw::xMin() {
  if (m_parameter2 == 0.0f) {
    return m_parameter1 - 1.0f;
  }
  return m_parameter1 - 5.0f*m_parameter2;
}

float NormalLaw::xMax() {
  if (m_parameter2 == 0.0f) {
    return m_parameter1 + 1.0f;
  }
  return m_parameter1 + 5.0f*m_parameter2;
}

float NormalLaw::yMin() {
  return -0.2f;
}

float NormalLaw::yMax() {
  return 1.0f;
}

}
