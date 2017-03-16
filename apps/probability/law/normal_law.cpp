#include "normal_law.h"
#include <assert.h>
#include <math.h>
#include <float.h>
#include <ion.h>

namespace Probability {

NormalLaw::NormalLaw() :
  TwoParameterLaw(0.0f, 1.0f)
{
}

I18n::Message NormalLaw::title() {
  return I18n::Message::NormalLaw;
}

Law::Type NormalLaw::type() const {
  return Type::Normal;
}

bool NormalLaw::isContinuous() const {
  return true;
}

I18n::Message NormalLaw::parameterNameAtIndex(int index) {
  assert(index >= 0 && index < 2);
  if (index == 0) {
    return I18n::Message::Mu;
  } else {
    return I18n::Message::Sigma;
  }
}

I18n::Message NormalLaw::parameterDefinitionAtIndex(int index) {
  assert(index >= 0 && index < 2);
  if (index == 0) {
    return I18n::Message::MeanDefinition;
  } else {
    return I18n::Message::DeviationDefinition;
  }
}

float NormalLaw::xMin() {
  if (m_parameter2 == 0.0f) {
    return m_parameter1 - 1.0f;
  }
  return m_parameter1 - 5.0f*fabsf(m_parameter2);
}

float NormalLaw::xMax() {
  if (m_parameter2 == 0.0f) {
    return m_parameter1 + 1.0f;
  }
  return m_parameter1 + 5.0f*fabsf(m_parameter2);
}

float NormalLaw::yMin() {
  return -k_displayBottomMarginRatio*yMax();
}

float NormalLaw::yMax() {
  float maxAbscissa = m_parameter1;
  float result = evaluateAtAbscissa(maxAbscissa);
  if (isnan(result) || result <= 0.0f) {
    result = 1.0f;
  }
  return result*(1.0f+ k_displayTopMarginRatio);
}

float NormalLaw::evaluateAtAbscissa(float x) const {
  if (m_parameter2 == 0.0f) {
    return NAN;
  }
  return (1.0f/(fabsf(m_parameter2)*sqrtf(2.0f*M_PI)))*expf(-0.5f*powf((x-m_parameter1)/m_parameter2,2));
}

bool NormalLaw::authorizedValueAtIndex(float x, int index) const {
  return true;
}

float NormalLaw::cumulativeDistributiveFunctionAtAbscissa(float x) const {
  if (m_parameter2 ==  0.0f) {
    return NAN;
  }
  return standardNormalCumulativeDistributiveFunctionAtAbscissa((x-m_parameter1)/fabsf(m_parameter2));
}

float NormalLaw::cumulativeDistributiveInverseForProbability(float * probability) {
  if (m_parameter2 ==  0.0f) {
    return NAN;
  }
  return standardNormalCumulativeDistributiveInverseForProbability(*probability)*fabsf(m_parameter2) + m_parameter1;
}

float NormalLaw::standardNormalCumulativeDistributiveFunctionAtAbscissa(float abscissa) const {
  if (abscissa == 0.0f) {
    return 0.5f;
  }
  if (abscissa < 0.0f) {
    return 1.0f - standardNormalCumulativeDistributiveFunctionAtAbscissa(-abscissa);
  }
  if (abscissa > k_boundStandardNormalDistribution) {
    return 1.0f;
  }
  /* Waissi & Rossin's formula (error less than 0.0001) */
  return 1.0f/(1.0f+expf(-sqrtf(M_PI)*(k_beta1*powf(abscissa,5)+k_beta2*powf(abscissa,3)+k_beta3*abscissa)));
}

float NormalLaw::standardNormalCumulativeDistributiveInverseForProbability(float probability) {
  if (probability >= 1.0f) {
    return INFINITY;
  }
  if (probability <= 0.0f) {
    return -INFINITY;
  }
  if (probability < 0.5f) {
    return -standardNormalCumulativeDistributiveInverseForProbability(1-probability);
  }
  /* Soranzo & Epure (error less than 0.001) */
  return (k_alpha3/logf(k_alpha2))*logf(1.0f - logf(-logf(probability)/logf(2.0f))/logf(k_alpha1));
}


}
