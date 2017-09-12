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
  return m_parameter1 - 5.0f*fabs(m_parameter2);
}

float NormalLaw::xMax() {
  if (m_parameter2 == 0.0f) {
    return m_parameter1 + 1.0f;
  }
  return m_parameter1 + 5.0f*fabs(m_parameter2);
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
  return (1.0f/(fabs(m_parameter2)*sqrt(2.0f*M_PI)))*exp(-0.5f*pow((x-m_parameter1)/m_parameter2,2));
}

bool NormalLaw::authorizedValueAtIndex(float x, int index) const {
  if (index == 0) {
    return true;
  }
  if (x <= FLT_MIN || fabs(m_parameter1/x) > k_maxRatioMuSigma) {
    return false;
  }
  return true;
}

void NormalLaw::setParameterAtIndex(float f, int index) {
  TwoParameterLaw::setParameterAtIndex(f, index);
  if (index == 0 && fabs(m_parameter1/m_parameter2) > k_maxRatioMuSigma) {
    m_parameter2 = m_parameter1/k_maxRatioMuSigma;
  }
}

double NormalLaw::cumulativeDistributiveFunctionAtAbscissa(double x) const {
  if (m_parameter2 ==  0.0f) {
    return NAN;
  }
  return standardNormalCumulativeDistributiveFunctionAtAbscissa((x-m_parameter1)/fabs(m_parameter2));
}

double NormalLaw::cumulativeDistributiveInverseForProbability(double * probability) {
  if (m_parameter2 ==  0.0f) {
    return NAN;
  }
  return standardNormalCumulativeDistributiveInverseForProbability(*probability)*fabs(m_parameter2) + m_parameter1;
}

double NormalLaw::standardNormalCumulativeDistributiveFunctionAtAbscissa(double abscissa) const {
  if (abscissa == 0.0) {
    return 0.5;
  }
  if (abscissa < 0.0) {
    return 1.0 - standardNormalCumulativeDistributiveFunctionAtAbscissa(-abscissa);
  }
  if (abscissa > k_boundStandardNormalDistribution) {
    return 1.0;
  }
  /* Waissi & Rossin's formula (error less than 0.0001) */
  return 1.0/(1.0+exp(-sqrt(M_PI)*(k_beta1*pow(abscissa,5.0)+k_beta2*pow(abscissa,3.0)+k_beta3*abscissa)));
}

double NormalLaw::standardNormalCumulativeDistributiveInverseForProbability(double probability) {
  if (probability >= 1.0) {
    return INFINITY;
  }
  if (probability <= 0.0) {
    return -INFINITY;
  }
  if (probability < 0.5) {
    return -standardNormalCumulativeDistributiveInverseForProbability(1-probability);
  }
  /* Soranzo & Epure (error less than 0.001) */
  return (k_alpha3/log(k_alpha2))*log(1.0 - log(-log(probability)/log(2.0))/log(k_alpha1));
}


}
