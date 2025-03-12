#ifndef POINCARE_STATISTICS_INFERENCE_CHI2_TEST_H
#define POINCARE_STATISTICS_INFERENCE_CHI2_TEST_H

#include <poincare/src/statistics/data_table.h>

#include "significance_test.h"

namespace Poincare::Internal::Inference::SignificanceTest {

namespace Chi2 {

bool IsObservedValueValid(double value);
bool IsExpectedValueValid(double value);
bool AreHomogeneityInputsValid(const DataTable* observedValues);
bool AreGoodnessInputsValid(const DataTable* observedValues,
                            const DataTable* expectedValues);

void FillHomogeneityExpectedValues(const DataTable* observedValues,
                                   DataTable* expectedValues);

bool IsDegreesOfFreedomValid(double p);
int ComputeHomogeneityDegreesOfFreedom(const DataTable* observedValues);
int ComputeGoodnessOfFitDegreesOfFreedom(const DataTable* observedValues);

void FillContributions(const DataTable* observedValues,
                       const DataTable* expectedValues,
                       DataTable* contributions);
double ComputeCriticalValue(const DataTable* contributions);

}  // namespace Chi2

}  // namespace Poincare::Internal::Inference::SignificanceTest

#endif
