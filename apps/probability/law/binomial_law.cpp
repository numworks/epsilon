#include "binomial_law.h"
#include <assert.h>
#include <math.h>

namespace Probability {

BinomialLaw::BinomialLaw() :
  TwoParameterLaw()
{
}

const char * BinomialLaw::title() {
  return "Loi binomiale";
}

Law::Type BinomialLaw::type() const {
  return Type::Binomial;
}

bool BinomialLaw::isContinuous() {
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
  return 0.0f;
}

float BinomialLaw::xMax() {
  if (m_parameter1 == 0) {
    return 1.0f;
  }
  return m_parameter1;
}

float BinomialLaw::yMin() {
  return -0.2f;
}

float BinomialLaw::yMax() {
  return 1.0f;
}

float BinomialLaw::evaluateAtAbscissa(float x) const {
  //m_expression = Expression::parse("binomial(a, b)*b^t*(1-b)^(a-t)");
  return powf(m_parameter2, (int)x)*powf(1-m_parameter2, m_parameter1-(int)x);
}

}
