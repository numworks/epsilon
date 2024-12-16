#include "slope_t_test.h"

#include <poincare/k_tree.h>

#include "apps/i18n.h"
#include "omg/unreachable.h"

namespace Inference {

/* TODO: estimateValue, estimateLayout and estimateDescription should be
 * refactored into one function returning a single object containing the value,
 * the symbol and the description. */

double SlopeTTest::estimateValue(int index) {
  assert(index >= 0 && index < numberOfEstimates());
  switch (index) {
    case 0:
      return slope(m_series);
    case 1:
      return computeStandardError();
    default:
      OMG::unreachable();
  }
}

Poincare::Layout SlopeTTest::estimateLayout(int index) const {
  assert(index >= 0 && index < numberOfEstimates());
  switch (index) {
    case 0:
      return KRackL(KCodePointL<'b'>());
    case 1:
      return Poincare::Layout::String("SE");
    default:
      OMG::unreachable();
  }
}

I18n::Message SlopeTTest::estimateDescription(int index) {
  assert(index >= 0 && index < numberOfEstimates());
  switch (index) {
    case 0:
      return I18n::Message::SampleSlope;
    case 1:
      return I18n::Message::StandardError;
    default:
      OMG::unreachable();
  }
}

void SlopeTTest::compute() {
  double n = doubleCastedNumberOfPairsOfSeries(m_series);
  double b = slope(m_series);
  m_degreesOfFreedom = n - 2.0;
  m_testCriticalValue = b / computeStandardError();
  m_pValue = SignificanceTest::ComputePValue(this);
}

Shared::ParameterRepresentation SlopeTTest::paramRepresentationAtIndex(
    int i) const {
  return Shared::ParameterRepresentation{KRackL(), I18n::Message::Default};
}

}  // namespace Inference
