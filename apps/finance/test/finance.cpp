#include <quiz.h>
#include <poincare/test/helper.h>
#include <float.h>
#include "../data.h"

using namespace Finance;

constexpr double k_precision = 5e-3;

void assert_interest_solves(const double * expectedValues, bool booleanParam, InterestData * data) {
  // Set all parameters
  data->m_booleanParam = booleanParam;
  for (uint8_t paramIndex = 0; paramIndex < data->numberOfDoubleValues(); paramIndex++) {
    data->setValue(paramIndex, expectedValues[paramIndex]);
  }
  // Test each parameters
  for (uint8_t paramIndex = 0; paramIndex < data->numberOfUnknowns(); paramIndex++) {
    uint8_t previousUnknownParamIndex = data->getUnknown();
    // Set new unknown parameter, previous unknown parameter's value is resetted
    data->setUnknown(paramIndex);
    // Restore previous unknown parameter's value
    if (previousUnknownParamIndex < data->numberOfDoubleValues() && previousUnknownParamIndex != paramIndex) {
      data->setValue(previousUnknownParamIndex, expectedValues[previousUnknownParamIndex]);
    }
    // Compute and check unknown parameter's value
    double expectedValue = expectedValues[paramIndex];

    assert_roughly_equal(data->computeUnknownValue(), expectedValue, k_precision, false);
  }
}

QUIZ_CASE(equation_finance_simple_interest) {
  SimpleInterestData data;
  {
    const double values[SimpleInterestData::k_numberOfDoubleValues] = {
      100.0,      // n
      1.0,        // rPct
      -1000.0,    // P
      2.777777778 // I
    };

    bool yearConventionIs360 = true;
    assert_interest_solves(values, yearConventionIs360, &data);
  }
  {
    const double values[SimpleInterestData::k_numberOfDoubleValues] = {
      365.0,      // n
      10.0,       // rPct
      1000.0,     // P
      -100.0      // I
    };
    bool yearConventionIs360 = false;
    assert_interest_solves(values, yearConventionIs360, &data);
  }
}

QUIZ_CASE(equation_finance_compound_interest) {
  CompoundInterestData data;
  {
    const double values[CompoundInterestData::k_numberOfDoubleValues] = {
      72.0,       // N
      12.55741064,// rPct
      12600.0,    // PV
      -250.0,     // Pmt
      0.0,        // FV
      12.0,       // PY
      12.0        // CY
    };
    bool paymentIsAtBegining = false;
    assert_interest_solves(values, paymentIsAtBegining, &data);
  }
  {
    const double values[CompoundInterestData::k_numberOfDoubleValues] = {
      8.0,        // N
      6.4,        // rPct
      -37000.0,   // PV
      0.0,        // Pmt
      42009.9,    // FV
      4.0,        // PY
      4.0         // CY
    };
    bool paymentIsAtBegining = false;
    assert_interest_solves(values, paymentIsAtBegining, &data);
  }
  {
    const double values[CompoundInterestData::k_numberOfDoubleValues] = {
      22.0,       // N
      0.0,        // rPct
      -24000.0,   // PV
      1000.0,     // Pmt
      2000.0,     // FV
      4.0,        // PY
      4.0         // CY
    };
    bool paymentIsAtBegining = false;
    assert_interest_solves(values, paymentIsAtBegining, &data);
  }
  {
    const double values[CompoundInterestData::k_numberOfDoubleValues] = {
      18.9692,    // N
      6.4,        // rPct
      -37000.0,   // PV
      0.0,        // Pmt
      50000.0,    // FV
      4.0,        // PY
      4.0         // CY
    };
    bool paymentIsAtBegining = false;
    assert_interest_solves(values, paymentIsAtBegining, &data);
  }
  {
    const double values[CompoundInterestData::k_numberOfDoubleValues] = {
      6.0,        // N
      0.0,        // rPct
      0.0,        // PV
      -1.0,       // Pmt
      6.0,        // FV
      12.0,       // PY
      1.0         // CY
    };
    bool paymentIsAtBegining = true;
    assert_interest_solves(values, paymentIsAtBegining, &data);
  }
  {
    const double values[CompoundInterestData::k_numberOfDoubleValues] = {
      120.0,      // N
      5.0,        // rPct
      0.0,        // PV
      100.0,      // Pmt
      -15511.0514,// FV
      12.0,       // PY
      4.0         // CY
    };
    bool paymentIsAtBegining = false;
    assert_interest_solves(values, paymentIsAtBegining, &data);
  }
}
