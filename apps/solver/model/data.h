#ifndef SOLVER_MODELS_DATA_H
#define SOLVER_MODELS_DATA_H

#include <stdint.h>
#include <apps/i18n.h>

namespace Solver {

constexpr static uint8_t k_numberOfSimpleInterestParameters = 5;

enum class SimpleInterestParameter : uint8_t {
  n = 0,
  rPct,
  P,
  I,
  YearConvention
};

// TODO Hugo : Move methods in sources

static_assert(static_cast<uint8_t>(SimpleInterestParameter::YearConvention) == k_numberOfSimpleInterestParameters-1, "YearConvention must be last.");

class SimpleInterestData {
public:
  static I18n::Message LabelForParameter(SimpleInterestParameter param) {
    uint8_t index = static_cast<uint8_t>(param);
    assert(index < k_numberOfSimpleInterestParameters);
    constexpr I18n::Message k_labels[k_numberOfSimpleInterestParameters] = {
      I18n::Message::FinanceLowerN,
      I18n::Message::FinanceRPct,
      I18n::Message::FinanceP,
      I18n::Message::FinanceI,
      I18n::Message::FinanceYear};
    return k_labels[index];
  }
  static I18n::Message SublabelForParameter(SimpleInterestParameter param) {
    uint8_t index = static_cast<uint8_t>(param);
    assert(index < k_numberOfSimpleInterestParameters);
    constexpr I18n::Message k_sublabels[k_numberOfSimpleInterestParameters] = {
      I18n::Message::NumberOfDays,
      I18n::Message::AnnualInterestRate,
      I18n::Message::InitialPrincipal,
      I18n::Message::FinalInterestAmount,
      I18n::Message::NumberOfDaysConvention};
    return k_sublabels[index];
  }
  static bool CheckValue(SimpleInterestParameter param, double value) {
    // TODO Hugo : Implement each parameter's validity check
    assert(param < SimpleInterestParameter::YearConvention);
    return !std::isnan(value);
  }
  void setValue(SimpleInterestParameter param, double value) {
    assert(param < SimpleInterestParameter::YearConvention);
    m_values[static_cast<uint8_t>(param)] = value;
  }
  double getValue(SimpleInterestParameter param) const {
    assert(param < SimpleInterestParameter::YearConvention);
    return m_values[static_cast<uint8_t>(param)];
  }
  SimpleInterestParameter m_unknown;
  bool m_yearConventionIs360;
private:
  constexpr static int k_numberOfDoubleValues = static_cast<int>(SimpleInterestParameter::YearConvention);
  // TODO Hugo : Add default values
  double m_values[k_numberOfDoubleValues];
};

// TODO Hugo : Update CompoundInterest as well

enum class CompoundInterestUnknown : uint8_t {
  None = 0,
  N,
  rPct,
  PV,
  Pmt,
  FV
};

class CompoundInterestData {
public:
  CompoundInterestUnknown m_unknown;
  double m_N;
  double m_rPct;
  double m_PV;
  double m_Pmt;
  double m_FV;
  double m_PY;
  double m_CY;
  bool m_paymentIsEnd;
};

union FinanceDataUnion {
  SimpleInterestData m_simpleInterestData;
  CompoundInterestData m_compoundInterestData;
};

class FinanceData {
public:
  bool isSimpleInterest;
  FinanceDataUnion m_data;
};

}  // namespace Solver

#endif /* SOLVER_MODELS_DATA_H */
