#include "binomial_law.h"
#include <assert.h>
#include <math.h>

namespace Probability {

BinomialLaw::BinomialLaw() :
  TwoParameterLaw(20.0f, 0.5f)
{
}

const char * BinomialLaw::title() {
  return "Loi binomiale";
}

Law::Type BinomialLaw::type() const {
  return Type::Binomial;
}

bool BinomialLaw::isContinuous() const {
  return false;
}

const char * BinomialLaw::parameterNameAtIndex(int index) {
  assert(index >= 0 && index < 2);
  if (index == 0) {
    return "n";
  } else {
    return "p";
  }
}

const char * BinomialLaw::parameterDefinitionAtIndex(int index) {
  assert(index >= 0 && index < 2);
  if (index == 0) {
    return "n : nombre de repetitions";
  } else {
    return "p : probabilites de succes";
  }
}

float BinomialLaw::xMin() {
  float min = floorf(m_parameter1*m_parameter2-5.0f*sqrtf(m_parameter1*m_parameter2*(1-m_parameter2)));
  float max = xMax();
  return min - k_displayLeftMarginRatio * (max - min);
}

float BinomialLaw::xMax() {
  float min = floorf(m_parameter1*m_parameter2-5.0f*sqrtf(m_parameter1*m_parameter2*(1-m_parameter2)));
  float max = ceilf(m_parameter1*m_parameter2+5.0f*sqrtf(m_parameter1*m_parameter2*(1-m_parameter2)));
  if (max <= min) {
    max = min + 1.0f;
  }
  return max + k_displayRightMarginRatio*(max - min);
}

float BinomialLaw::yMin() {
  return -k_displayBottomMarginRatio*yMax();
}

float BinomialLaw::yMax() {
  int maxAbscissa = m_parameter2 < 1.0f ? (m_parameter1+1)*m_parameter2 : m_parameter1;
  float result = evaluateAtAbscissa(maxAbscissa);
  if (result <= 0.0f || isnan(result)) {
    result = 1.0f;
  }
  return result*(1.0f+ k_displayTopMarginRatio);
}

float BinomialLaw::evaluateAtAbscissa(float x) const {
  if (m_parameter1 == 0.0f) {
    if (m_parameter2 == 0.0f || m_parameter2 == 1.0f) {
      return NAN;
    }
    if ((int)x == 0) {
      return 1.0f;
    }
    return 0.0f;
  }
  if (m_parameter2 == 1.0f) {
    if ((int)x == m_parameter1) {
      return 1.0f;
    }
    return 0.0f;
  }
  if (m_parameter2 == 0.0f) {
    if ((int)x == 0) {
      return 1.0f;
    }
    return 0.0f;
  }
  if (x > m_parameter1) {
    return 0.0f;
  }
  float lResult = lgammaf(m_parameter1+1) - lgammaf((int)x+1) - lgammaf(m_parameter1 - (int)x+1)+
    (int)x*logf(m_parameter2) + (m_parameter1-(int)x)*logf(1-m_parameter2);
  return expf(lResult);
}

bool BinomialLaw::authorizedValueAtIndex(float x, int index) const {
  if (index == 0) {
    if (x != (int)x) {
      return false;
    }
    return true;
  }
  if (x < 0.0f || x > 1.0f) {
    return false;
  }
  return true;
}

float BinomialLaw::cumulativeDistributiveInverseForProbability(float * probability) {
  if (m_parameter1 == 0.0f && (m_parameter2 == 0.0f || m_parameter2 == 1.0f)) {
    return NAN;
  }
  if (*probability >= 1.0f) {
    return m_parameter1;
  }
  return Law::cumulativeDistributiveInverseForProbability(probability);
}

float BinomialLaw::rightIntegralInverseForProbability(float * probability) {
  if (m_parameter1 == 0.0f && (m_parameter2 == 0.0f || m_parameter2 == 1.0f)) {
    return NAN;
  }
  if (*probability <= 0.0f) {
    return m_parameter1;
  }
  return Law::rightIntegralInverseForProbability(probability);
}


}
