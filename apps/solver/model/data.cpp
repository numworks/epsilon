#include "data.h"
#include <assert.h>
#include <stdint.h>
#include <cmath>

namespace Solver {

I18n::Message SimpleInterestData::LabelForParameter(SimpleInterestParameter param) {
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

I18n::Message SimpleInterestData::SublabelForParameter(SimpleInterestParameter param) {
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

double SimpleInterestData::DefaultValue(SimpleInterestParameter param) {
  assert(param < SimpleInterestParameter::YearConvention);
  uint8_t index = static_cast<uint8_t>(param);
  constexpr double k_defaultValues[k_numberOfSimpleInterestParameters-1] = {
    100.0,
    1.0,
    -1000.0,
    2.7};
  return k_defaultValues[index];
}

bool SimpleInterestData::CheckValue(SimpleInterestParameter param, double value) {
  assert(param < SimpleInterestParameter::YearConvention);
  return !std::isnan(value) && (param != SimpleInterestParameter::n || value >= 0);
}

void SimpleInterestData::resetValues() {
  m_yearConventionIs360 = true;
  setValue(SimpleInterestParameter::n, NAN);
  m_unknown = SimpleInterestParameter::n;
  setValue(SimpleInterestParameter::rPct, DefaultValue(SimpleInterestParameter::rPct));
  setValue(SimpleInterestParameter::P, DefaultValue(SimpleInterestParameter::P));
  setValue(SimpleInterestParameter::I, DefaultValue(SimpleInterestParameter::I));
}

void SimpleInterestData::setValue(SimpleInterestParameter param, double value) {
  assert(param < SimpleInterestParameter::YearConvention);
  m_values[static_cast<uint8_t>(param)] = value;
}

double SimpleInterestData::getValue(SimpleInterestParameter param) const {
  if (param == m_unknown) {
    assert(std::isnan(m_values[static_cast<uint8_t>(param)]));
    // TODO Hugo : Compute it and return
    return 1234.0;
  }
  assert(param < SimpleInterestParameter::YearConvention);
  return m_values[static_cast<uint8_t>(param)];
}

void SimpleInterestData::setUnknown(SimpleInterestParameter param) {
  if (param != m_unknown) {
    setValue(m_unknown, DefaultValue(m_unknown));
    m_unknown = param;
    setValue(m_unknown, NAN);
  }
}

}  // namespace Solver
