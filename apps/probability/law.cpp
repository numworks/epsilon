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

float Law::parameterValueAtIndex(int index) {
  assert(index >= 0 && index < 2);
  if (index == 0) {
    return m_parameter1;
  }
  return m_parameter2;
}

const char * Law::parameterNameAtIndex(int index) {
  assert(index >= 0 && index < 2);
  switch (m_type) {
    case Law::Type::Binomial:
      if (index == 0) {
        return "n";
      } else {
        return "p";
      }
    case Law::Type::Uniform:
      if (index == 0) {
        return "a";
      } else {
        return "b";
      }
    case Law::Type::Exponential:
      return "l";
    case Law::Type::Normal:
      if (index == 0) {
        return "u";
      } else {
        return "o";
      }
    case Law::Type::Poisson:
      return "l";
    default:
      return 0;
  }
}

const char * Law::parameterDefinitionAtIndex(int index) {
  assert(index >= 0 && index < 2);
  switch (m_type) {
    case Law::Type::Binomial:
      if (index == 0) {
        return "n : nombre de repetitions";
      } else {
        return "p : probabilites de succes";
      }
    case Law::Type::Uniform:
      if (index == 0) {
        return "[a, b] intervalle";
      } else {
        return nullptr;
      }
    case Law::Type::Exponential:
      return "l : parametre";
    case Law::Type::Normal:
      if (index == 0) {
        return "u : moyenne";
      } else {
        return "o : ecart-type";
      }
    case Law::Type::Poisson:
      return "l : parametre";
    default:
      return 0;
  }
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
