#include "law.h"
#include <math.h>

namespace Probability {

Law::Law(EvaluateContext * evaluateContext):
  m_evaluateContext(evaluateContext)
{
}

EvaluateContext * Law::evaluateContext() {
  return m_evaluateContext;
}

float Law::gridUnit() {
  int a = 0;
  int b = 0;
  float d = xMax() - xMin();
  float units[3] = {k_oneUnit, k_twoUnit, k_fiveUnit};
  for (int k = 0; k < 3; k++) {
    float unit = units[k];
    if (floorf(log10f(d/(unit*k_maxNumberOfXGridUnits))) != floorf(log10f(d/(unit*k_minNumberOfXGridUnits)))) {
      b = floorf(log10f(d/(unit*k_minNumberOfXGridUnits)));
      a = unit;
    }
  }
  return a*powf(10,b);
}

float Law::evaluateAtAbscissa(float t) const {
  m_evaluateContext->setOverridenValueForSymbolT(t);
  return expression()->approximate(*m_evaluateContext);
}

}
