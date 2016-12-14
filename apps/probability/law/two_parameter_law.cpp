#include "two_parameter_law.h"
#include <assert.h>

namespace Probability {

TwoParameterLaw::TwoParameterLaw(Context * context) :
  Law(context),
  m_parameter1(0.0f),
  m_parameter2(0.0f)
{
  Symbol aSymbol = Symbol('a');
  Float e1 = Float(m_parameter1);
  m_context->setExpressionForSymbolName(&e1, &aSymbol);
  Symbol bSymbol = Symbol('b');
  Float e2 = Float(m_parameter2);
  m_context->setExpressionForSymbolName(&e2, &bSymbol);
}

int TwoParameterLaw::numberOfParameter() {
  return 2;
}

float TwoParameterLaw::parameterValueAtIndex(int index) {
  assert(index >= 0 && index < 2);
  if (index == 0) {
    return m_parameter1;
  }
  return m_parameter2;
}

void TwoParameterLaw::setParameterAtIndex(float f, int index) {
  assert(index >= 0 && index < 2);
  if (index == 0) {
    m_parameter1 = f;
    Symbol aSymbol = Symbol('a');
    Float e1 = Float(f);
    m_context->setExpressionForSymbolName(&e1, &aSymbol);
  } else {
    m_parameter2 = f;
    Symbol bSymbol = Symbol('b');
    Float e2 = Float(f);
    m_context->setExpressionForSymbolName(&e2, &bSymbol);
  }
}

}
