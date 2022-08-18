#include "data.h"
#include <poincare/solver.h>
#include <stdint.h>
#include <cmath>

namespace Finance {

/* InterestData */

void InterestData::setUnknown(uint8_t param) {
  assert(m_unknown < numberOfUnknowns());
  m_unknown = param;
}

void InterestData::resetValues() {
  m_booleanParam = false;
  // Set first parameter as unknown by default
  m_unknown = 0;
  // Default the other double parameters
  for (uint8_t param = 0; param < numberOfDoubleValues(); param++) {
    setValue(param, defaultValue(param));
  }
}

/* SimpleInterestData */

I18n::Message SimpleInterestData::labelForParameter(uint8_t param) const {
  assert(param < numberOfParameters());
  constexpr I18n::Message k_labels[k_numberOfParameters] = {
      I18n::Message::FinanceLowerN, I18n::Message::FinanceRPct,
      I18n::Message::FinanceP, I18n::Message::FinanceI,
      I18n::Message::FinanceYear};
  return k_labels[param];
}

I18n::Message SimpleInterestData::sublabelForParameter(uint8_t param) const {
  assert(param < numberOfParameters());
  constexpr I18n::Message k_sublabels[k_numberOfParameters] = {
      I18n::Message::NumberOfDays, I18n::Message::AnnualInterestRate,
      I18n::Message::InitialPrincipal, I18n::Message::FinalInterestAmount,
      I18n::Message::NumberOfDaysConvention};
  return k_sublabels[param];
}

double SimpleInterestData::defaultValue(uint8_t param) const {
  assert(param < numberOfDoubleValues());
  constexpr double k_defaultValues[k_numberOfDoubleValues] = {100.0, 1.0,
                                                              -1000.0, 2.7};
  return k_defaultValues[param];
}

bool SimpleInterestData::checkValue(uint8_t param, double value) const {
  assert(param < numberOfDoubleValues());
  return !std::isnan(value)
         && (param != static_cast<uint8_t>(Parameter::n) || value >= 0);
}

double SimpleInterestData::computeUnknownValue() {
  /* Using the formula
   * I = -P * r * n'
   * With rPct = r * 100
   *      n = n' * 360 (or 365 depending on year convention)
   */
  double year = m_booleanParam ? 360.0 : 365.0;
  double I = getValue(static_cast<uint8_t>(Parameter::Int));
  double rPct = getValue(static_cast<uint8_t>(Parameter::rPct));
  double P = getValue(static_cast<uint8_t>(Parameter::P));
  double n = getValue(static_cast<uint8_t>(Parameter::n));
  double result;
  switch (static_cast<Parameter>(m_unknown)) {
    case Parameter::n:
      result = -I * 100.0 * year / (rPct * P);
      break;
    case Parameter::rPct:
      result = -I * 100.0 * year / (n * P);
      break;
    case Parameter::P:
      result = -I * 100.0 * year / (rPct * n);
      break;
    default:
      assert(m_unknown == static_cast<uint8_t>(Parameter::Int));
      result = -rPct * n * P / (100.0 * year);
  }
  if (!std::isfinite(result)) {
    // Prevent 0 divisions from returning inf
    result = NAN;
  }
  setValue(m_unknown, result);
  return result;
}

/* CompoundInterestData */

I18n::Message CompoundInterestData::labelForParameter(uint8_t param) const {
  assert(param < numberOfParameters());
  constexpr I18n::Message k_labels[k_numberOfParameters] = {
      I18n::Message::FinanceN,  I18n::Message::FinanceRPct,
      I18n::Message::FinancePV, I18n::Message::FinancePmt,
      I18n::Message::FinanceFV, I18n::Message::FinancePY,
      I18n::Message::FinanceCY, I18n::Message::FinancePayment};
  return k_labels[param];
}

I18n::Message CompoundInterestData::sublabelForParameter(uint8_t param) const {
  assert(param < numberOfParameters());
  constexpr I18n::Message k_sublabels[k_numberOfParameters] = {
      I18n::Message::NumberOfPayments,
      I18n::Message::NominalAnnualInterestRate,
      I18n::Message::PresentValue,
      I18n::Message::PaymentEachPeriod,
      I18n::Message::FutureValue,
      I18n::Message::NumberPaymentYear,
      I18n::Message::NumberCompoundingPeriodYear,
      I18n::Message::BeginningEndPeriod};
  return k_sublabels[param];
}

double CompoundInterestData::defaultValue(uint8_t param) const {
  assert(param < numberOfDoubleValues());
  constexpr double k_defaultValues[k_numberOfDoubleValues] = {
      0.0, 0.0, 0.0, 0.0, 0.0, 12.0, 12.0};
  return k_defaultValues[param];
}

bool CompoundInterestData::checkValue(uint8_t param, double value) const {
  assert(param < numberOfDoubleValues());
  return !std::isnan(value)
         && ((param != static_cast<uint8_t>(Parameter::N)
              && param != static_cast<uint8_t>(Parameter::CY)
              && param != static_cast<uint8_t>(Parameter::PY))
             || value >= 0);
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

double computeN(double rPct,
                double PV,
                double Pmt,
                double FV,
                double S,
                double i) {
  if (rPct == 0.0) {
    return -(PV + FV) / Pmt;
  }
  return std::log(((1.0 + i * S) * Pmt - FV * i)
                  / ((1.0 + i * S) * Pmt + PV * i))
         / std::log(1.0 + i);
}

double computeRPct(double N,
                   double PV,
                   double Pmt,
                   double FV,
                   double PY,
                   double CY,
                   double S) {
  if (Pmt == 0.0) {
    // PV + FV*(1 + r/(100*CY))^(-N*CY/PY) = 0
    return 100.0 * CY * (std::pow(-FV / PV, PY / (N * CY)) - 1.0);
  }
  const double parameters[7] = {N, PV, Pmt, FV, PY, CY, S};
  // We must solve this expression. An exact solution cannot be found.
  Poincare::Solver::ValueAtAbscissa evaluation =
      [](double x, Poincare::Context *, const void * aux) {
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
  return Poincare::Solver::NextRoot(
      evaluation, nullptr, parameters, -100.0, 100,
      Poincare::Solver::k_relativePrecision, Poincare::Solver::k_minimalStep,
      Poincare::Solver::DefaultMaximalStep(-100.0, 100));
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

double CompoundInterestData::computeUnknownValue() {
  /* Using the formulas
   * PV + α*Pmt + β*FV = 0
   * With α = (1 + i*S) * (1-β)/i
   *      β = (1 + i)^(-N)
   *      S = 1 if m_booleanParam, 0 otherwise
   *      i = (1 + rPct/(100*CY))^(CY/PY) - 1
   * If rPct is 0, α = N and β = 1
   * If Pmt is 0, PY = CY and FV = PV * (1 + rPct/100CY)^N
   */
  double N = getValue(static_cast<uint8_t>(Parameter::N));
  double rPct = getValue(static_cast<uint8_t>(Parameter::rPct));
  double PV = getValue(static_cast<uint8_t>(Parameter::PV));
  double Pmt = getValue(static_cast<uint8_t>(Parameter::Pmt));
  double FV = getValue(static_cast<uint8_t>(Parameter::FV));
  double CY = getValue(static_cast<uint8_t>(Parameter::CY));
  double PY = getValue(static_cast<uint8_t>(Parameter::PY));
  double S = (m_booleanParam ? 1.0 : 0.0);
  double i = computeI(rPct, CY, PY);
  double b = computeB(i, N);
  double a = computeA(i, S, b, N);
  double result;
  switch (static_cast<Parameter>(m_unknown)) {
    case Parameter::N:
      result = computeN(rPct, PV, Pmt, FV, S, i);
      break;
    case Parameter::rPct:
      result = computeRPct(N, PV, Pmt, FV, PY, CY, S);
      break;
    case Parameter::PV:
      result = computePV(Pmt, FV, a, b);
      break;
    case Parameter::Pmt:
      result = computePmt(PV, FV, a, b);
      break;
    default:
      assert(m_unknown == static_cast<uint8_t>(Parameter::FV));
      result = computeFV(PV, Pmt, a, b);
  }
  if (!std::isfinite(result)) {
    // Prevent 0 divisions from returning inf
    result = NAN;
  }
  setValue(m_unknown, result);
  return result;
}

void CompoundInterestData::setValue(uint8_t param, double value) {
  assert(param < numberOfDoubleValues());
  if (param == static_cast<uint8_t>(Parameter::PY)) {
    // Updating PY should also update CY
    m_values[static_cast<uint8_t>(Parameter::CY)] = value;
  }
  m_values[param] = value;
}

}  // namespace Finance
