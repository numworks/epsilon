#ifndef SOLVER_MODELS_DATA_H
#define SOLVER_MODELS_DATA_H

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

static_assert(static_cast<uint8_t>(SimpleInterestParameter::YearConvention) == k_numberOfSimpleInterestParameters-1, "YearConvention must be last.");

class SimpleInterestData {
public:
  static I18n::Message LabelForParameter(SimpleInterestParameter param);
  static I18n::Message SublabelForParameter(SimpleInterestParameter param);
  static double DefaultValue(SimpleInterestParameter param);
  static bool CheckValue(SimpleInterestParameter param, double value);
  void resetValues();
  void setValue(SimpleInterestParameter param, double value);
  double getValue(SimpleInterestParameter param) const;
  void setUnknown(SimpleInterestParameter param);
  SimpleInterestParameter getUnknown() const { return m_unknown; }
  bool m_yearConventionIs360;
private:
  constexpr static int k_numberOfDoubleValues = static_cast<int>(SimpleInterestParameter::YearConvention);

  SimpleInterestParameter m_unknown;
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
