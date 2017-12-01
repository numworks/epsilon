#include "poisson_law.h"
#include <assert.h>
#include <cmath>
#include <ion.h>

namespace Probability {

PoissonLaw::PoissonLaw() :
  OneParameterLaw(4.0f)
{
}

I18n::Message PoissonLaw::title() {
  return I18n::Message::PoissonLaw;
}

Law::Type PoissonLaw::type() const {
  return Type::Poisson;
}

bool PoissonLaw::isContinuous() const {
  return false;
}

I18n::Message PoissonLaw::parameterNameAtIndex(int index) {
  assert(index == 0);
  return I18n::Message::Lambda;
}

I18n::Message PoissonLaw::parameterDefinitionAtIndex(int index) {
  assert(index == 0);
  return I18n::Message::LambdaPoissonDefinition;
}

float PoissonLaw::xMin() {
  return -k_displayLeftMarginRatio*xMax();
}

float PoissonLaw::xMax() {
  assert(m_parameter1 != 0);
  return (m_parameter1 + 5.0f*std::sqrt(m_parameter1))*(1.0f+k_displayRightMarginRatio);
}

float PoissonLaw::yMin() {
  return - k_displayBottomMarginRatio * yMax();
}

float PoissonLaw::yMax() {
  int maxAbscissa = (int)m_parameter1;
  assert(maxAbscissa >= 0.0f);
  float result = evaluateAtAbscissa(maxAbscissa);
  if (result <= 0.0f) {
    result = 1.0f;
  }
  return result*(1.0f+ k_displayTopMarginRatio);
}

bool PoissonLaw::authorizedValueAtIndex(float x, int index) const {
  if (x <= 0.0f || x > 999.0f) {
    return false;
  }
  return true;
}

template<typename T>
T PoissonLaw::templatedApproximateAtAbscissa(T x) const {
  if (x < 0) {
    return NAN;
  }
  T lResult = -(T)m_parameter1+std::floor(x)*std::log((T)m_parameter1)-std::lgamma(std::floor(x)+1);
  return std::exp(lResult);
}

}

template float Probability::PoissonLaw::templatedApproximateAtAbscissa(float x) const;
template double Probability::PoissonLaw::templatedApproximateAtAbscissa(double x) const;
