#include "data.h"
#include <poincare/solver.h>
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
  /* Using the formula
   * I = -P * r * n'
   * With rPct = r * 100
   *      n = n' * 360 (or 365 depending on year convention)
   */
  double year = m_yearConventionIs360 ? 360.0 : 365.0;
  double I = getValue(SimpleInterestParameter::I);
  double rPct = getValue(SimpleInterestParameter::rPct);
  double P = getValue(SimpleInterestParameter::P);
  double n = getValue(SimpleInterestParameter::n);
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
    72.0,
    12.5,
    14000.0,
    -250.0,
    0.0,
    12.0,
    12.0};
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

double computeI(double rPct, double CY, double PY) {
  return std::pow(1.0 + rPct / (100.0 * CY), CY / PY) - 1.0;
}

double computeB(double i, double N) {
  return std::pow(1.0 + i, -N);
}

double computeA(double i, double S, double b, double N) {
  return i == 0.0 ? N : (1.0 + i * S) * (1.0 - b) / i;
}

double computeN(double rPct, double PV, double Pmt, double FV, double S, double i) {
  if (rPct == 0.0) {
    return -(PV + FV) / Pmt;
  }
  return std::log(((1.0 + i * S) * Pmt - FV * i)
                  / ((1.0 + i * S) * Pmt + PV * i))
         / std::log(1.0 + i);
}

double computeRPct(double N, double PV, double Pmt, double FV, double PY, double CY, double S) {
  if (Pmt == 0.0) {
    // PV + FV*(1 + r/(100*CY))^(-N*CY/PY) = 0
    assert(CY == PY);
    return 100.0 * CY * (std::pow(-FV/PV, 1/N) - 1.0);
  }
  const double parameters[7] = {N, PV, Pmt, FV, PY, CY, S};
  // We must solve this expression. An exact solution cannot be found.
  Poincare::Solver::ValueAtAbscissa evaluation = [](double x, Poincare::Context *, const void * aux) {
    const double * pack = static_cast<const double *>(aux);
    double N = pack[0];
    double PV = pack[1];
    double Pmt = pack[2];
    double FV = pack[3];
    double PY = pack[4];
    double CY = pack[5];
    double S = pack[6];
    double i = computeI(x, CY, PY);
    double b = computeB(i, N);
    double a = computeA(i, S, b, N);
    return PV + a * Pmt + b * FV;
  };
  return Poincare::Solver::NextRoot(evaluation, nullptr, parameters, -100.0, 100, Poincare::Solver::k_relativePrecision, Poincare::Solver::k_minimalStep, Poincare::Solver::DefaultMaximalStep(-100.0, 100));
}

double computePV(double Pmt, double FV, double a, double b) {
  return -a * Pmt - b * FV;
}

double computePmt(double PV, double FV, double a, double b) {
  return -(PV + b * FV) / a;
}

double computeFV(double PV, double Pmt, double a, double b) {
  return -(PV + a * Pmt) / b;
}

double CompoundInterestData::computeUnknownValue() const {
  /* Using the formulas
   * PV + α*Pmt + β*FV = 0
   * With α = (1 + i*S) * (1-β)/i
   *      β = (1 + i)^(-N)
   *      S = 1 if m_paymentIsBeginning, 0 otherwise
   *      i = (1 + rPct/(100*CY))^(CY/PY) - 1
   * If rPct is 0, α = N and β = 1
   * If Pmt is 0, PY = CY and FV = PV * (1 + rPct/100CY)^N
   */
  double N = getValue(CompoundInterestParameter::N);
  double rPct = getValue(CompoundInterestParameter::rPct);
  double PV = getValue(CompoundInterestParameter::PV);
  double Pmt = getValue(CompoundInterestParameter::Pmt);
  double FV = getValue(CompoundInterestParameter::FV);
  double CY = getValue(CompoundInterestParameter::CY);
  // Discard PY so that PY/CY is 1 if Pmt is null.
  double PY = (Pmt == 0.0) ? CY : getValue(CompoundInterestParameter::PY);
  double S = (m_paymentIsBeginning ? 1.0 : 0.0);
  double i = computeI(rPct, CY, PY);
  double b = computeB(i, N);
  double a = computeA(i, S, b, N);
  double result;
  switch (m_unknown) {
    case CompoundInterestParameter::N:
      result = computeN(rPct, PV, Pmt, FV, S, i);
      break;
    case CompoundInterestParameter::rPct:
      result = computeRPct(N, PV, Pmt, FV, PY, CY, S);
      break;
    case CompoundInterestParameter::PV:
      result = computePV(Pmt, FV, a, b);
      break;
    case CompoundInterestParameter::Pmt:
      result = computePmt(PV, FV, a, b);
      break;
    default:
      assert(m_unknown == CompoundInterestParameter::FV);
      result = computeFV(PV, Pmt, a, b);
  }
  if (!std::isfinite(result)) {
    // Prevent 0 divisions from returning inf
    return NAN;
  }
  return result;
}

}  // namespace Solver
