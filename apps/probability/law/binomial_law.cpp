#include "binomial_law.h"
#include <assert.h>

namespace Probability {

BinomialLaw::BinomialLaw(EvaluateContext * evaluateContext) :
  TwoParameterLaw(evaluateContext),
  m_expression(Expression::parse("p1-p2*t"))
{
  //m_expression = Expression::parse("binomial(p1, p2)*p2^t*(1-p2)^(p1-t)");
  assert(m_expression != nullptr);
}

BinomialLaw::~BinomialLaw() {
  delete m_expression;
}

const char * BinomialLaw::title() {
  return "Loi binomiale";
}

Expression * BinomialLaw::expression() const {
  return m_expression;
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

}
