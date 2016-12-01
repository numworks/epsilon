#include "law.h"
#include <assert.h>

namespace Probability {

Law::Law():
  m_type(Law::Type::NoType),
  m_parameter1(0.0f),
  m_parameter2(0.0f),
  m_expression(nullptr),
  m_xMin(-10.0f),
  m_xMax(10.0f)
{
}

int Law::numberOfParameter() {
  switch (m_type) {
    case Law::Type::Binomial:
      return 2;
    case Law::Type::Uniform:
      return 2;
    case Law::Type::Exponential:
      return 1;
    case Law::Type::Normal:
      return 2;
    case Law::Type::Poisson:
      return 1;
    default:
      return 0;
  }
}

float Law::parameterAtIndex(int index) {
  assert(index >= 0 && index < 2);
  if (index == 0) {
    return m_parameter1;
  }
  return m_parameter2;
}

void Law::setParameterAtIndex(float f, int index) {
  assert(index >= 0 && index < 2);
  if (index == 0) {
    m_parameter1 = f;
    return;
  }
  m_parameter2 = f;
}

float Law::evaluateAtAbscissa(float x, Graph::EvaluateContext * context) const {
  context->setOverridenValueForSymbolX(x);
  return m_expression->approximate(*context);
}

Law::Type Law::type() const {
  return m_type;
}

void Law::setType(Type type) {
  m_type = type;
}

bool Law::isContinuous() {
  switch (m_type) {
    case Law::Type::Binomial:
      return false;
    case Law::Type::Uniform:
      return true;
    case Law::Type::Exponential:
      return true;
    case Law::Type::Normal:
      return true;
    case Law::Type::Poisson:
      return false;
    default:
      return false;
  }
}

}
