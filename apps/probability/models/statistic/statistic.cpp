#include "statistic.h"

#include "probability/app.h"

namespace Probability {

bool Statistic::testPassed() {
   return pValue() > App::app()->snapshot()->data()->hypothesisParams()->firstParam();
}

} // namespace Probability
