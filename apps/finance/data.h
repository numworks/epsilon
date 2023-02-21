#ifndef FINANCE_DATA_H
#define FINANCE_DATA_H

#include <apps/i18n.h>
#include <assert.h>

#include <new>

namespace Finance {

class InterestData {
 public:
  constexpr static uint8_t k_numberOfSharedDoubleValues = 4;
  constexpr static uint8_t k_maxNumberOfUnknowns =
      5;  // static_cast<uint8_t>(CompoundInterestData::Parameter::PY)

  InterestData(double *sharedValues) : m_sharedValues(sharedValues) {}

  virtual I18n::Message labelForParameter(uint8_t param) const = 0;
  virtual I18n::Message sublabelForParameter(uint8_t param) const = 0;
  virtual I18n::Message menuTitle() const = 0;
  virtual double defaultValue(uint8_t param) const;
  virtual bool checkValue(uint8_t param, double value) const = 0;
  virtual uint8_t numberOfParameters() const = 0;
  virtual uint8_t numberOfDoubleValues() const = 0;
  virtual uint8_t numberOfUnknowns() const = 0;
  virtual I18n::Message dropdownMessageAtIndex(int index) const = 0;
  virtual double computeUnknownValue() = 0;
  virtual void setValue(uint8_t param, double value) {
    assert(param < k_numberOfSharedDoubleValues);
    m_sharedValues[param] = value;
  }
  virtual double getValue(uint8_t param) const {
    assert(param < k_numberOfSharedDoubleValues);
    return m_sharedValues[param];
  }

  void setUnknown(uint8_t param) {
    assert(m_unknown < numberOfUnknowns());
    m_unknown = param;
  }
  uint8_t getUnknown() const { return m_unknown; }
  void resetValues();
  /* For SimpleInterestData, this param tells if the convention for the number
   * of days per year is 360 (365 otherwise). For CompoundInterestData, it tells
   * if the payment is made at the beginning of the payment period (end
   * otherwise). */
  bool m_booleanParam;

 protected:
  uint8_t m_unknown;

 private:
  double *m_sharedValues;
};

class SimpleInterestData : public InterestData {
 private:
  enum class Parameter : uint8_t {
    n = 0,          // Shared value with CompoundInterestData::N
    rPct,           // Shared value with CompoundInterestData::rPct
    P,              // Shared value with CompoundInterestData::PV
    Int,            // Shared value with CompoundInterestData::FV
    YearConvention  // Is not a double parameter and can't be unknown
  };

 public:
  SimpleInterestData(double *sharedValues) : InterestData(sharedValues) {
    resetValues();
  }

  constexpr static uint8_t k_numberOfParameters = 5;
  static_assert(static_cast<uint8_t>(Parameter::n) == 0,
                "n must be first to match CompoundInterestData::N.");
  static_assert(static_cast<uint8_t>(Parameter::rPct) == 1,
                "rPct must be second to match CompoundInterestData::rPct.");
  static_assert(static_cast<uint8_t>(Parameter::P) == 2,
                "P must be third to match CompoundInterestData::PV.");
  static_assert(static_cast<uint8_t>(Parameter::Int) == 3,
                "Int must be forth to match CompoundInterestData::FV.");
  static_assert(static_cast<uint8_t>(Parameter::YearConvention) ==
                    k_numberOfParameters - 1,
                "YearConvention must be last.");

  constexpr static uint8_t k_numberOfDoubleValues =
      static_cast<uint8_t>(Parameter::YearConvention);
  static_assert(k_numberOfDoubleValues - k_numberOfSharedDoubleValues == 0,
                "All double values are expected to be shared.");
  constexpr static uint8_t k_numberOfUnknowns = k_numberOfDoubleValues;
  static_assert(k_maxNumberOfUnknowns >= k_numberOfUnknowns,
                "k_maxNumberOfUnknowns is invalid.");

  I18n::Message labelForParameter(uint8_t param) const override;
  I18n::Message sublabelForParameter(uint8_t param) const override;
  I18n::Message menuTitle() const override {
    return I18n::Message::SimpleInterest;
  }
  bool checkValue(uint8_t param, double value) const override;
  uint8_t numberOfParameters() const override { return k_numberOfParameters; }
  uint8_t numberOfDoubleValues() const override {
    return k_numberOfDoubleValues;
  }
  uint8_t numberOfUnknowns() const override { return k_numberOfUnknowns; }
  I18n::Message dropdownMessageAtIndex(int index) const override {
    return index == 0 ? I18n::Message::Finance360 : I18n::Message::Finance365;
  }
  double computeUnknownValue() override;
};

class CompoundInterestData : public InterestData {
 private:
  enum class Parameter : uint8_t {
    N = 0,  // Shared value with SimpleInterestData::n
    rPct,   // Shared value with SimpleInterestData::rPct
    PV,     // Shared value with SimpleInterestData::P
    FV,     // Shared value with SimpleInterestData::Int
    Pmt,
    PY,      // Can't be unknown
    CY,      // Can't be unknown
    Payment  // Is not a double parameter and can't be unknown
  };

 public:
  CompoundInterestData(double *sharedValues) : InterestData(sharedValues) {
    resetValues();
  }

  constexpr static uint8_t k_numberOfParameters = 8;
  static_assert(static_cast<uint8_t>(Parameter::N) == 0,
                "N must be first to match SimpleInterestData::n.");
  static_assert(static_cast<uint8_t>(Parameter::rPct) == 1,
                "rPct must be second to match SimpleInterestData::rPct.");
  static_assert(static_cast<uint8_t>(Parameter::PV) == 2,
                "PV must be third to match SimpleInterestData::P.");
  static_assert(static_cast<uint8_t>(Parameter::FV) == 3,
                "FV must be forth to match SimpleInterestData::Int.");
  static_assert(static_cast<uint8_t>(Parameter::PY) == k_numberOfParameters - 3,
                "PY must be third from last.");
  static_assert(static_cast<uint8_t>(Parameter::CY) == k_numberOfParameters - 2,
                "CY must be second from last.");
  static_assert(static_cast<uint8_t>(Parameter::Payment) ==
                    k_numberOfParameters - 1,
                "Payment must be last.");

  constexpr static uint8_t k_numberOfDoubleValues =
      static_cast<uint8_t>(Parameter::Payment);
  constexpr static uint8_t k_numberOfUnknowns =
      static_cast<uint8_t>(Parameter::PY);
  static_assert(k_maxNumberOfUnknowns >= k_numberOfUnknowns,
                "k_maxNumberOfUnknowns is invalid.");

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
  double computeUnknownValue() override;
  void setValue(uint8_t param, double value) override;
  double getValue(uint8_t param) const override;

 private:
  double m_values[k_numberOfDoubleValues - k_numberOfSharedDoubleValues];
};

class Data {
 public:
  // By default, select the simple interest data model
  Data()
      : m_compoundInterestData(m_sharedValues),
        m_simpleInterestData(m_sharedValues),
        m_selectedModel(true) {}
  void reset();
  void setModel(bool selectedModel) { m_selectedModel = selectedModel; }
  InterestData *interestData() {
    return m_selectedModel
               ? static_cast<InterestData *>(&m_simpleInterestData)
               : static_cast<InterestData *>(&m_compoundInterestData);
  }

 private:
  CompoundInterestData m_compoundInterestData;
  SimpleInterestData m_simpleInterestData;
  double m_sharedValues[InterestData::k_numberOfSharedDoubleValues];
  bool m_selectedModel;
};

}  // namespace Finance

#endif
