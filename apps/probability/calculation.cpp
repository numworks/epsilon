#include "calculation.h"
#include <assert.h>
#include <math.h>

namespace Probability {

Calculation::Calculation():
  m_type(LeftIntegral),
  m_parameter1(0.0f),
  m_parameter2(0.0f),
  m_parameter3(0.0f),
  m_law(nullptr)
{
}

void Calculation::setLaw(Law * law) {
  m_law = law;
  computeCalculation(0);
}

void Calculation::setType(Type type) {
  m_type = type;
  initParameters();
}

Calculation::Type Calculation::type() const {
  return m_type;
}

float Calculation::parameterAtIndex(int index) {
  if (index == 0) {
    return m_parameter1;
  }
  if (index == 1) {
    return m_parameter2;
  }
  return m_parameter3;
}

void Calculation::setParameterAtIndex(float f, int index) {
  if (index == 0) {
    m_parameter1 = f;
  }
  if (index == 1) {
    m_parameter2 = f;
  }
  if (index == 2) {
    m_parameter3 = f;
  }
  computeCalculation(index);
}

void Calculation::computeCalculation(int indexKnownElement) {
  if (m_law == nullptr) {
    return;
  }
  if (m_type == LeftIntegral) {
    if (indexKnownElement == 0) {
      // TODO: compute integral from -Inf to m_parameter1
      m_parameter2 = m_law->evaluateAtAbscissa(m_parameter1);
    } else {
      // TODO: find m_parameter1
      m_parameter1 = m_law->evaluateAtAbscissa(m_parameter2);
    }
  }
  if (m_type == FiniteIntegral) {
    // TODO: compute integral from m_parameter1 to m_parameter2
    m_parameter3 = m_law->evaluateAtAbscissa(m_parameter1 + m_parameter2);
  }
  if (m_type == RightIntegral) {
    if (indexKnownElement == 0) {
      // TODO: compute integral from m_parameter1 to +Inf
      m_parameter2 = m_law->evaluateAtAbscissa(m_parameter1);
    } else {
      // TODO: find m_parameter1
      m_parameter1 = m_law->evaluateAtAbscissa(m_parameter2);
    }
  }
}

void Calculation::initParameters() {
  if (m_type == LeftIntegral) {
    m_parameter1 = 0.0f;
    computeCalculation(0);
  }
  if (m_type == FiniteIntegral) {
    m_parameter1 = -1.0f;
    m_parameter2 = 1.0f;
    computeCalculation(0);
  }
  if (m_type == RightIntegral) {
    m_parameter1 = 0.0f;
    computeCalculation(0);
  }
}

}
