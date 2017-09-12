#include "exponential_law.h"
#include <assert.h>
#include <cmath>
#include <float.h>
#include <ion.h>

namespace Probability {

ExponentialLaw::ExponentialLaw() :
  OneParameterLaw(1.0f)
{
}

I18n::Message ExponentialLaw::title() {
  return I18n::Message::ExponentialLaw;
}

Law::Type ExponentialLaw::type() const {
  return Type::Exponential;
}

bool ExponentialLaw::isContinuous() const {
  return true;
}

I18n::Message ExponentialLaw::parameterNameAtIndex(int index) {
  assert(index == 0);
  return I18n::Message::Lambda;
}

I18n::Message ExponentialLaw::parameterDefinitionAtIndex(int index) {
  assert(index == 0);
  return I18n::Message::LambdaExponentialDefinition;
}

float ExponentialLaw::xMin() {
  float max = xMax();
  return - k_displayLeftMarginRatio * max;
}

float ExponentialLaw::xMax() {
  assert(m_parameter1 != 0.0f);
  float result = 5.0f/m_parameter1;
  if (result <= 0.0f) {
    result = 1.0f;
  }
  return result*(1.0f+ k_displayRightMarginRatio);
}

float ExponentialLaw::yMin() {
  return -k_displayBottomMarginRatio*yMax();
}

float ExponentialLaw::yMax() {
  float result = m_parameter1;
  if (result <= 0.0f || std::isnan(result)) {
    result = 1.0f;
  }
  if (result <= 0.0f) {
    result = 1.0f;
  }
  return result*(1.0f+ k_displayTopMarginRatio);
}

float ExponentialLaw::evaluateAtAbscissa(float x) const {
  if (x < 0.0f) {
    return NAN;
  }
  return m_parameter1*std::exp(-m_parameter1*x);
}

bool ExponentialLaw::authorizedValueAtIndex(float x, int index) const {
  if (x <= 0.0f || x > 7500.0f) {
    return false;
  }
  return true;
}

double ExponentialLaw::cumulativeDistributiveFunctionAtAbscissa(double x) const {
  return 1.0 - std::exp((double)(-m_parameter1*x));
}

double ExponentialLaw::cumulativeDistributiveInverseForProbability(double * probability) {
  if (*probability >= 1.0) {
    return INFINITY;
  }
  if (*probability <= 0.0) {
    return 0.0;
  }
  return -std::log(1.0 - *probability)/(double)m_parameter1;
}

}
