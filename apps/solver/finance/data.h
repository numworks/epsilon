#ifndef SOLVER_DATA_H
#define SOLVER_DATA_H

#include <apps/i18n.h>
#include <assert.h>

namespace Solver {

class InterestData {
public:
  // 5 = static_cast<uint8_t>(CompoundInterestData::Parameter::PY)
  constexpr static uint8_t k_maxNumberOfUnknowns = 5;

  virtual I18n::Message labelForParameter(uint8_t param) const = 0;
  virtual I18n::Message sublabelForParameter(uint8_t param) const = 0;
  virtual I18n::Message menuTitle() const = 0;
  virtual double defaultValue(uint8_t param) const = 0;
  virtual bool checkValue(uint8_t param, double value) const = 0;
  virtual uint8_t numberOfParameters() const = 0;
  virtual uint8_t numberOfDoubleValues() const = 0;
  virtual uint8_t numberOfUnknowns() const = 0;
  virtual I18n::Message dropdownMessageAtIndex(int index) const = 0;
  virtual double computeUnknownValue() const = 0;

  void resetValues();
  void setValue(uint8_t param, double value) {
    assert(param < numberOfDoubleValues());
    m_values[param] = value;
  }
  double getValue(uint8_t param) const {
    assert(param < numberOfDoubleValues());
    return m_values[param];
  }
  void setUnknown(uint8_t param);
  uint8_t getUnknown() const { return m_unknown; }
  // Each FinanceData has one boolean parameter
  bool m_booleanParam;

protected:
  // 7 = static_cast<uint8_t>(CompoundInterestData::Parameter::Payment)
  constexpr static uint8_t k_maxNumberOfDoubleValues = 7;
  uint8_t m_unknown;

private:
  double m_values[k_maxNumberOfDoubleValues];
};

class SimpleInterestData : public InterestData {
public:
  constexpr static uint8_t k_numberOfParameters = 5;
  enum class Parameter : uint8_t {
    n = 0,
    rPct,
    P,
    I,
    YearConvention  // Is not a double parameter and can't be unknown
  };
  static_assert(static_cast<uint8_t>(Parameter::YearConvention) == k_numberOfParameters - 1, "YearConvention must be last.");

  constexpr static uint8_t k_numberOfDoubleValues = static_cast<uint8_t>(Parameter::YearConvention);
  static_assert(k_maxNumberOfDoubleValues >= k_numberOfDoubleValues, "k_maxNumberOfDoubleValues is invalid.");

  constexpr static uint8_t k_numberOfUnknowns = k_numberOfDoubleValues;
  static_assert(k_maxNumberOfUnknowns >= k_numberOfUnknowns, "k_maxNumberOfUnknowns is invalid.");

  I18n::Message labelForParameter(uint8_t param) const override;
  I18n::Message sublabelForParameter(uint8_t param) const override;
  I18n::Message menuTitle() const override {
    return I18n::Message::SimpleInterest;
  }
  double defaultValue(uint8_t param) const override;
  bool checkValue(uint8_t param, double value) const override;
  uint8_t numberOfParameters() const override { return k_numberOfParameters; }
  uint8_t numberOfDoubleValues() const override {
    return k_numberOfDoubleValues;
  }
  uint8_t numberOfUnknowns() const override { return k_numberOfUnknowns; }
  I18n::Message dropdownMessageAtIndex(int index) const override {
    return index == 0 ? I18n::Message::Finance360 : I18n::Message::Finance365;
  }
  double computeUnknownValue() const override;
};

class CompoundInterestData : public InterestData {
public:
  constexpr static uint8_t k_numberOfParameters = 8;
  enum class Parameter : uint8_t {
    N = 0,
    rPct,
    PV,
    Pmt,
    FV,
    PY,      // Can't be unknown
    CY,      // Can't be unknown
    Payment  // Is not a double parameter and can't be unknown
  };
  static_assert(static_cast<uint8_t>(Parameter::PY) == k_numberOfParameters - 3, "PY must be third from last.");
  static_assert(static_cast<uint8_t>(Parameter::CY) == k_numberOfParameters - 2, "CY must be second from last.");
  static_assert(static_cast<uint8_t>(Parameter::Payment) == k_numberOfParameters - 1, "Payment must be last.");

  constexpr static uint8_t k_numberOfDoubleValues = static_cast<uint8_t>(Parameter::Payment);
  static_assert(k_maxNumberOfDoubleValues >= k_numberOfDoubleValues, "k_maxNumberOfDoubleValues is invalid.");

  constexpr static uint8_t k_numberOfUnknowns = static_cast<uint8_t>(Parameter::PY);
  static_assert(k_maxNumberOfUnknowns >= k_numberOfUnknowns, "k_maxNumberOfUnknowns is invalid.");

  I18n::Message labelForParameter(uint8_t param) const override;
  I18n::Message sublabelForParameter(uint8_t param) const override;
  I18n::Message menuTitle() const override {
    return I18n::Message::CompoundInterest;
  }
  double defaultValue(uint8_t param) const override;
  bool checkValue(uint8_t param, double value) const override;
  uint8_t numberOfParameters() const override { return k_numberOfParameters; }
  uint8_t numberOfDoubleValues() const override {
    return k_numberOfDoubleValues;
  }
  uint8_t numberOfUnknowns() const override { return k_numberOfUnknowns; }
  I18n::Message dropdownMessageAtIndex(int index) const override {
    return index == 0 ? I18n::Message::FinanceBeginning
                      : I18n::Message::FinanceEnd;
  }
  double computeUnknownValue() const override;
};

// TODO : Create an union for the financial data

class FinanceData {
public:
  InterestData * getInterestData(bool simpleInterest) {
    if (simpleInterest) {
      return &m_simpleInterestData;
    }
    return &m_compoundInterestData;
  }

private:
  SimpleInterestData m_simpleInterestData;
  CompoundInterestData m_compoundInterestData;
};

}  // namespace Solver

#endif /* SOLVER_DATA_H */
