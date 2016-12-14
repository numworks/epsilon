#include "one_parameter_law.h"
#include <assert.h>

namespace Probability {

OneParameterLaw::OneParameterLaw(Context * context) :
  Law(context),
  m_parameter1(0.5f)
{
  Symbol aSymbol = Symbol('a');
  Float e = Float(m_parameter1);
  m_context->setExpressionForSymbolName(&e, &aSymbol);
}

int OneParameterLaw::numberOfParameter() {
  return 1;
}

float OneParameterLaw::parameterValueAtIndex(int index) {
  assert(index == 0);
  return m_parameter1;
}

void OneParameterLaw::setParameterAtIndex(float f, int index) {
  assert(index == 0);
  m_parameter1 = f;
  Symbol aSymbol = Symbol('a');
  Float e = Float(f);
  m_context->setExpressionForSymbolName(&e, &aSymbol);
}

}
