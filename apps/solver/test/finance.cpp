#include <quiz.h>
#include <poincare/test/helper.h>
#include <float.h>
#include "../finance/data.h"

using namespace Solver;

constexpr double k_precision = 1e-3;
constexpr double k_reference = FLT_EPSILON;

void assert_interest_solves(const double * expectedValues, bool booleanParam, InterestData * data, double reference = k_reference) {
  // Set all parameters
  data->m_booleanParam = booleanParam;
  for (uint8_t paramIndex = 0; paramIndex < data->numberOfDoubleValues(); paramIndex++) {
    data->setValue(paramIndex, expectedValues[paramIndex]);
  }
  // Test each parameters
  for (uint8_t paramIndex = 0; paramIndex < data->numberOfUnknowns(); paramIndex++) {
    uint8_t previousUnknownParam = data->getUnknown();
    // Set new unknown parameter, previous unknown parameter's value is resetted
    data->setUnknown(paramIndex);
    // Restore previous unknown parameter's value
    uint8_t previousUnknownParamIndex = previousUnknownParam;
    if (previousUnknownParamIndex < data->numberOfDoubleValues() && previousUnknownParamIndex != paramIndex) {
      data->setValue(previousUnknownParam, expectedValues[previousUnknownParamIndex]);
    }
    // Compute and check unknown parameter's value
    double expectedValue = expectedValues[paramIndex];

    quiz_assert(IsApproximatelyEqual(data->computeUnknownValue(), expectedValue, k_precision, reference));
  }
  data->resetValues();
}

QUIZ_CASE(equation_finance_simple_interest) {
  // n, rPct, P, I
  SimpleInterestData data;
  {
    const double values[SimpleInterestData::k_numberOfDoubleValues] = {
      100.0, 1.0, -1000.0, 2.777777778};
    assert_interest_solves(values, true, &data);
  }
  {
    const double values[SimpleInterestData::k_numberOfDoubleValues] = {
      365.0, 10.0, 1000.0, -100.0};
    assert_interest_solves(values, false, &data);
  }
}

QUIZ_CASE(equation_finance_compound_interest) {
  // N, rPct, PV, Pmt, FV, PY, CY
  CompoundInterestData data;
  {
    const double values[CompoundInterestData::k_numberOfDoubleValues] = {
      72.0, 12.55741064, 12600.0, -250.0, 0.0, 12.0, 12.0};
    assert_interest_solves(values, false, &data, 10.0);
  }
  {
    const double values[CompoundInterestData::k_numberOfDoubleValues] = {
      8.0, 6.4, -37000.0, 0.0, 42009.9, 4.0, 4.0};
    assert_interest_solves(values, false, &data, 10.0);
  }
  {
    const double values[CompoundInterestData::k_numberOfDoubleValues] = {
      22.0, 0.0, -24000.0, 1000.0, 2000.0, 4.0, 4.0};
    assert_interest_solves(values, false, &data, 0.1);
  }
  {
    const double values[CompoundInterestData::k_numberOfDoubleValues] = {
      18.9692, 6.4, -37000.0, 0.0, 50000.0, 4.0, 4.0};
    assert_interest_solves(values, false, &data, 1.0);
  }
}
