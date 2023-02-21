#include "../interval.h"

#include <quiz.h>

namespace Shared {

void testInterval(double start, double end, double step, double* elements,
                  int elementCount) {
  Interval testInterval;
  Interval::IntervalParameters* params = testInterval.parameters();
  params->setStart(start);
  params->setEnd(end);
  params->setStep(step);
  quiz_assert(testInterval.numberOfElements() == elementCount);
  for (int i = 0; i < elementCount; ++i) {
    quiz_assert(elements[i] == testInterval.element(i));
  }
}

QUIZ_CASE(interval_values) {
  {
    double elements[12] = {-1.2, -1.0, -0.8, -0.6, -0.4, -0.2,
                           0.0,  0.2,  0.4,  0.6,  0.8,  1.0};
    testInterval(-1.2, 1.1, 0.2, elements, sizeof(elements) / sizeof(double));
  }
  {
    double elements[12] = {-1e-16, -9e-17, -8e-17, -7e-17, -6e-17, -5e-17,
                           -4e-17, -3e-17, -2e-17, -1e-17, 0,      1e-17};
    testInterval(-1e-16, 1e-17, 1e-17, elements,
                 sizeof(elements) / sizeof(double));
  }
}

}  // namespace Shared