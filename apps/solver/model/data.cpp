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
  uint8_t index = static_cast<uint8_t>(param);
  assert(index < k_numberOfDoubleValues);
  constexpr double k_defaultValues[k_numberOfDoubleValues] = {
    100.0,
    1.0,
    -1000.0,
    2.7};
  return k_defaultValues[index];
}

bool SimpleInterestData::CheckValue(SimpleInterestParameter param, double value) {
  assert(param < static_cast<SimpleInterestParameter>(k_numberOfDoubleValues));
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
  uint8_t paramIndex = static_cast<uint8_t>(param);
  assert(paramIndex < k_numberOfDoubleValues);
  m_values[paramIndex] = value;
}

double SimpleInterestData::getValue(SimpleInterestParameter param) const {
  uint8_t paramIndex = static_cast<uint8_t>(param);
  assert(paramIndex < k_numberOfDoubleValues);
  return m_values[paramIndex];
}

void SimpleInterestData::setUnknown(SimpleInterestParameter param) {
  if (m_unknown >= static_cast<SimpleInterestParameter>(k_numberOfUnknowns)) {
    // Can happen because of the union
    m_unknown = param;
  } else if (param != m_unknown) {
    setValue(m_unknown, DefaultValue(m_unknown));
    m_unknown = param;
    setValue(m_unknown, NAN);
  }
}

double SimpleInterestData::computeUnknownValue() const {
  double year = m_yearConventionIs360 ? 360.0 : 365.0;
  double I = getValue(SimpleInterestParameter::I);
  double rPct = getValue(SimpleInterestParameter::rPct);
  double P = getValue(SimpleInterestParameter::P);
  double n = getValue(SimpleInterestParameter::n);
  /* Using the formula
   * I = -P * r * n'
   * With rPct = r * 100
   *      n = n' * 360 (or 365 depending on year convention)
   */
  double result;
  switch (m_unknown) {
  case SimpleInterestParameter::n :
    result = -I*100.0*year/(rPct*P);
    break;
  case SimpleInterestParameter::rPct :
    result = -I*100.0*year/(n*P);
    break;
  case SimpleInterestParameter::P :
    result = -I*100.0*year/(rPct*n);
    break;
  default:
    assert(m_unknown == SimpleInterestParameter::I);
    result = -rPct*n*P/(100.0*year);
  }
  if (!std::isfinite(result)) {
    // Prevent 0 divisions from returning inf
    return NAN;
  }
  return result;
}

I18n::Message CompoundInterestData::LabelForParameter(CompoundInterestParameter param) {
  uint8_t index = static_cast<uint8_t>(param);
  assert(index < k_numberOfCompoundInterestParameters);
  constexpr I18n::Message k_labels[k_numberOfCompoundInterestParameters] = {
    I18n::Message::FinanceN,
    I18n::Message::FinanceRPct,
    I18n::Message::FinancePV,
    I18n::Message::FinancePmt,
    I18n::Message::FinanceFV,
    I18n::Message::FinancePY,
    I18n::Message::FinanceCY,
    I18n::Message::FinancePayment};
  return k_labels[index];
}

I18n::Message CompoundInterestData::SublabelForParameter(CompoundInterestParameter param) {
  uint8_t index = static_cast<uint8_t>(param);
  assert(index < k_numberOfCompoundInterestParameters);
  constexpr I18n::Message k_sublabels[k_numberOfCompoundInterestParameters] = {
    I18n::Message::NumberOfPeriods,
    I18n::Message::NominalAnnualInterestRate,
    I18n::Message::PresentValue,
    I18n::Message::PaymentEachPeriod,
    I18n::Message::FutureValue,
    I18n::Message::NumberPaymentYear,
    I18n::Message::NumberCompoundingPeriodYear,
    I18n::Message::BeginningEndPeriod};
  return k_sublabels[index];
}

double CompoundInterestData::DefaultValue(CompoundInterestParameter param) {
  uint8_t index = static_cast<uint8_t>(param);
  assert(index < k_numberOfDoubleValues);
  constexpr double k_defaultValues[k_numberOfDoubleValues] = {
    24.0,
    6.4,
    0.0,
    -14000.0,
    0.0,
    4.0,
    2.7};
  return k_defaultValues[index];
}

bool CompoundInterestData::CheckValue(CompoundInterestParameter param, double value) {
  assert(param < static_cast<CompoundInterestParameter>(k_numberOfDoubleValues));
  return !std::isnan(value)
         && ((param != CompoundInterestParameter::N
              && param != CompoundInterestParameter::CY
              && param != CompoundInterestParameter::PY)
             || value >= 0);
}

void CompoundInterestData::resetValues() {
  m_paymentIsBeginning = true;
  setValue(CompoundInterestParameter::N, NAN);
  m_unknown = CompoundInterestParameter::N;
  setValue(CompoundInterestParameter::rPct, DefaultValue(CompoundInterestParameter::rPct));
  setValue(CompoundInterestParameter::PV, DefaultValue(CompoundInterestParameter::PV));
  setValue(CompoundInterestParameter::Pmt, DefaultValue(CompoundInterestParameter::Pmt));
  setValue(CompoundInterestParameter::FV, DefaultValue(CompoundInterestParameter::FV));
  setValue(CompoundInterestParameter::PY, DefaultValue(CompoundInterestParameter::PY));
  setValue(CompoundInterestParameter::CY, DefaultValue(CompoundInterestParameter::CY));
}

void CompoundInterestData::setValue(CompoundInterestParameter param, double value) {
  uint8_t paramIndex = static_cast<uint8_t>(param);
  assert(paramIndex < k_numberOfDoubleValues);
  m_values[paramIndex] = value;
}

double CompoundInterestData::getValue(CompoundInterestParameter param) const {
  uint8_t paramIndex = static_cast<uint8_t>(param);
  assert(paramIndex < k_numberOfDoubleValues);
  return m_values[paramIndex];
}

void CompoundInterestData::setUnknown(CompoundInterestParameter param) {
  if (m_unknown >= static_cast<CompoundInterestParameter>(k_numberOfUnknowns)) {
    // Can happen because of the union
    m_unknown = param;
  } else if (param != m_unknown) {
    setValue(m_unknown, DefaultValue(m_unknown));
    m_unknown = param;
    setValue(m_unknown, NAN);
  }
}

double CompoundInterestData::computeUnknownValue() const {
  double N = getValue(CompoundInterestParameter::N);
  double rPct = getValue(CompoundInterestParameter::rPct);
  double PV = getValue(CompoundInterestParameter::PV);
  double Pmt = getValue(CompoundInterestParameter::Pmt);
  double FV = getValue(CompoundInterestParameter::FV);
  double PY = getValue(CompoundInterestParameter::PY);
  double CY = getValue(CompoundInterestParameter::CY);

  /* Using the formulas
   * PV + α*Pmt + β*FV = 0
   * With α = (1 + i*S) * (1-β)/i
   *      β = (1 + i)^(-N)
   *      S = 1 if m_paymentIsBeginning, 0 otherwise
   *      i = (1 + rPct/(100*CY))^(CY/PY) - 1
   * If rPct is 0, α = N and β = 1
   */
  double i;
  double S = (m_paymentIsBeginning ? 1.0 : 0.0);
  double b;
  double a;
  if (rPct == 0.0) {
    i = 0.0;
    b = 1.0;
    a = N;
  } else {
    i = std::pow(1.0 + rPct / (100.0 * CY), CY / PY) - 1.0;
    b = std::pow(1.0 + i, -N);
    a = (1.0 + i * S) * (1.0 - b) / i;
  }
  double result;
  switch (m_unknown) {
    case CompoundInterestParameter::N:
      result = (rPct == 0.0) ? -(PV + FV) / Pmt
                           : std::log(((1.0 + i * S) * Pmt - FV * i)
                                      / ((1.0 + i * S) * Pmt + PV * i))
                                 / std::log(1.0 + i);
      break;
    case CompoundInterestParameter::rPct:
      // TODO : Solve rPct
      result = 0.0;
      break;
    case CompoundInterestParameter::PV:
      result = -a * Pmt - b * FV;
      break;
    case CompoundInterestParameter::Pmt:
      result = -(PV + b * FV) / a;
      break;
    default:
      assert(m_unknown == CompoundInterestParameter::FV);
      result = -(PV + a * Pmt) / b;
  }
  if (!std::isfinite(result)) {
    // Prevent 0 divisions from returning inf
    return NAN;
  }
  return result;
}

}  // namespace Solver
