#include "law.h"
#include <assert.h>
#include <math.h>

namespace Probability {

Law::Law(EvaluateContext * evaluateContext):
  m_type(Law::Type::NoType),
  m_parameter1(0.0f),
  m_parameter2(0.0f),
  m_expression(nullptr),
  m_xMin(-10.0f),
  m_xMax(10.0f),
  m_yMin(-0.1f),
  m_yMax(1.0f),
  m_evaluateContext(evaluateContext)
{
}

Law::~Law() {
  if (m_expression != nullptr) {
    delete m_expression;
  }
}

EvaluateContext * Law::evaluateContext() {
  return m_evaluateContext;
}

void Law::setType(Type type) {
  if (m_expression != nullptr) {
    delete m_expression;
    m_expression = nullptr;
  }
  const char * text = nullptr;
  switch (type) {
    // TODO: implement binomial, indicator function
    case Type::Binomial:
      text = "binomial(p1, p2)*p2^t*(1-p2)^(p1-t)";
      //break;
    case Type::Uniform:
      text = "1/(p2-p1)";
      //break;
    case Type::Exponential:
      text = "p1*exp(-p1*t)";
      //break;
    case Type::Normal:
      text = "(1/(p2*sqrt(2*Pi))*exp(-0.5*((t-p1)/p2)^2)";
      //break;
    case Type::Poisson:
      text = "exp(-p1)*p1^t/t!";
      text = "p1+p2*t";
      break;
    default:
      break;
  }
  if (text) {
    m_expression = Expression::parse(text);
  }
  setParameterAtIndex(0.0f, 0);
  setParameterAtIndex(0.0f, 1);
  m_type = type;
}

Law::Type Law::type() const {
  return m_type;
}

Expression * Law::expression() {
 return m_expression;
}

bool Law::isContinuous() {
  switch (m_type) {
    case Type::Binomial:
      return false;
    case Type::Uniform:
      return true;
    case Type::Exponential:
      return true;
    case Type::Normal:
      return true;
    case Type::Poisson:
      return false;
    default:
      return false;
  }
}

float Law::xMin() {
  return m_xMin;
}

float Law::yMin() {
  return m_yMin;
}

float Law::xMax() {
  return m_xMax;
}

float Law::yMax() {
  return m_yMax;
}

int Law::numberOfParameter() {
  switch (m_type) {
    case Type::Binomial:
      return 2;
    case Type::Uniform:
      return 2;
    case Type::Exponential:
      return 1;
    case Type::Normal:
      return 2;
    case Type::Poisson:
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
    // TODO: replace by greek letter
    case Type::Binomial:
      if (index == 0) {
        return "n";
      } else {
        return "p";
      }
    case Type::Uniform:
      if (index == 0) {
        return "a";
      } else {
        return "b";
      }
    case Type::Exponential:
      return "l";
    case Type::Normal:
      if (index == 0) {
        return "u";
      } else {
        return "o";
      }
    case Type::Poisson:
      return "l";
    default:
      return 0;
  }
}

const char * Law::parameterDefinitionAtIndex(int index) {
  assert(index >= 0 && index < 2);
  switch (m_type) {
    case Type::Binomial:
      if (index == 0) {
        return "n : nombre de repetitions";
      } else {
        return "p : probabilites de succes";
      }
    case Type::Uniform:
      if (index == 0) {
        return "[a, b] intervalle";
      } else {
        return nullptr;
      }
    case Type::Exponential:
      return "l : parametre";
    case Type::Normal:
      if (index == 0) {
        return "u : moyenne";
      } else {
        return "o : ecart-type";
      }
    case Type::Poisson:
      return "l : parametre";
    default:
      return 0;
  }
}

void Law::setParameterAtIndex(float f, int index) {
  assert(index >= 0 && index < 2);
  if (index == 0) {
    m_parameter1 = f;
    m_evaluateContext->setOverridenValueForFirstParameter(f);
  } else {
    m_parameter2 = f;
    m_evaluateContext->setOverridenValueForSecondParameter(f);
  }
  setWindow();
}

float Law::evaluateAtAbscissa(float t, EvaluateContext * context) const {
  context->setOverridenValueForSymbolT(t);
  return m_expression->approximate(*context);
}

void Law::setWindow() {
  switch (m_type) {
    case Type::Binomial:
      m_xMin = 0.0f;
      m_xMax = m_parameter1;
      break;
    case Type::Uniform:
      m_xMin = m_parameter1 - 3.0f;
      m_xMax = m_parameter2 + 3.0f;
      break;
    case Type::Exponential:
      m_xMin = 0.0f;
      m_xMax = 5.0f/m_parameter1;
      break;
    case Type::Normal:
      m_xMin = m_parameter1 - 5.0f*m_parameter2;
      m_xMax = m_parameter1 + 5.0f*m_parameter2;
      break;
    case Type::Poisson:
      m_xMin = 0.0f;
      m_xMax = m_parameter1 + 5.0f*sqrtf(m_parameter1);
    default:
      return;
  }
}

}
