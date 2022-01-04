#ifndef SOLVER_MODELS_DATA_H
#define SOLVER_MODELS_DATA_H

#include <apps/i18n.h>

namespace Solver {

// TODO : Factorize classes

constexpr static uint8_t k_numberOfSimpleInterestParameters = 5;

enum class SimpleInterestParameter : uint8_t {
  n = 0,
  rPct,
  P,
  I,
  YearConvention // Is not a double parameter and can't be unknown
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
  double computeUnknownValue() const;
  bool m_yearConventionIs360;
private:
  constexpr static int k_numberOfDoubleValues = static_cast<int>(SimpleInterestParameter::YearConvention);

  SimpleInterestParameter m_unknown;
  double m_values[k_numberOfDoubleValues];
};

constexpr static uint8_t k_numberOfCompoundInterestParameters = 8;

enum class CompoundInterestParameter : uint8_t {
  N,
  rPct,
  PV,
  Pmt,
  FV,
  PY, // Can't be unknown
  CY, // Can't be unknown
  Payment // Is not a double parameter and can't be unknown
};

static_assert(static_cast<uint8_t>(CompoundInterestParameter::Payment) == k_numberOfCompoundInterestParameters-1, "Payment must be last.");

class CompoundInterestData {
public:
  static I18n::Message LabelForParameter(CompoundInterestParameter param);
  static I18n::Message SublabelForParameter(CompoundInterestParameter param);
  static double DefaultValue(CompoundInterestParameter param);
  static bool CheckValue(CompoundInterestParameter param, double value);
  void resetValues();
  void setValue(CompoundInterestParameter param, double value);
  double getValue(CompoundInterestParameter param) const;
  void setUnknown(CompoundInterestParameter param);
  CompoundInterestParameter getUnknown() const { return m_unknown; }
  double computeUnknownValue() const;
  bool m_paymentIsBeginning;
private:
  constexpr static int k_numberOfDoubleValues = static_cast<int>(CompoundInterestParameter::Payment);

  CompoundInterestParameter m_unknown;
  double m_values[k_numberOfDoubleValues];
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
