#ifndef FINANCE_DATA_H
#define FINANCE_DATA_H

#include <apps/i18n.h>
#include <assert.h>

#include <new>

namespace Finance {

class InterestData {
public:
  constexpr static uint8_t k_maxNumberOfUnknowns = 5; // static_cast<uint8_t>(CompoundInterestData::Parameter::PY)

  virtual I18n::Message labelForParameter(uint8_t param) const = 0;
  virtual I18n::Message sublabelForParameter(uint8_t param) const = 0;
  virtual I18n::Message menuTitle() const = 0;
  virtual double defaultValue(uint8_t param) const = 0;
  virtual bool checkValue(uint8_t param, double value) const = 0;
  virtual uint8_t numberOfParameters() const = 0;
  virtual uint8_t numberOfDoubleValues() const = 0;
  virtual uint8_t numberOfUnknowns() const = 0;
  virtual I18n::Message dropdownMessageAtIndex(int index) const = 0;
  virtual double computeUnknownValue() = 0;
  virtual void setValue(uint8_t param, double value) = 0;
  virtual double getValue(uint8_t param) const = 0;

  void setUnknown(uint8_t param);
  uint8_t getUnknown() const { return m_unknown; }
  /* For SimpleInterestData, this param tells if the convention for the number
   * of days per year is 360 (365 otherwise). For CompoundInterestData, it tells
   * if the payment is made at the beginning of the payment period (end
   * otherwise). */
  bool m_booleanParam;

protected:
  void resetValues();
  uint8_t m_unknown;
};

class SimpleInterestData : public InterestData {
private:
  enum class Parameter : uint8_t {
    n = 0,
    rPct,
    P,
    Int,
    YearConvention  // Is not a double parameter and can't be unknown
  };
public:
  SimpleInterestData() { resetValues(); }

  constexpr static uint8_t k_numberOfParameters = 5;
  static_assert(static_cast<uint8_t>(Parameter::YearConvention) == k_numberOfParameters - 1, "YearConvention must be last.");

  constexpr static uint8_t k_numberOfDoubleValues = static_cast<uint8_t>(Parameter::YearConvention);
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
  double computeUnknownValue() override;
  void setValue(uint8_t param, double value) override {
    assert(param < numberOfDoubleValues());
    m_values[param] = value;
  }
  double getValue(uint8_t param) const override {
    assert(param < numberOfDoubleValues());
    return m_values[param];
  }

private:
  double m_values[k_numberOfDoubleValues];
};

class CompoundInterestData : public InterestData {
private:
  enum class Parameter : uint8_t {
    N = 0,
    rPct,
    PV,
    FV,
    Pmt,
    PY,      // Can't be unknown
    CY,      // Can't be unknown
    Payment  // Is not a double parameter and can't be unknown
  };
public:
  CompoundInterestData() { resetValues(); }

  constexpr static uint8_t k_numberOfParameters = 8;
  static_assert(static_cast<uint8_t>(Parameter::PY) == k_numberOfParameters - 3, "PY must be third from last.");
  static_assert(static_cast<uint8_t>(Parameter::CY) == k_numberOfParameters - 2, "CY must be second from last.");
  static_assert(static_cast<uint8_t>(Parameter::Payment) == k_numberOfParameters - 1, "Payment must be last.");

  constexpr static uint8_t k_numberOfDoubleValues = static_cast<uint8_t>(Parameter::Payment);
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
  double computeUnknownValue() override;
  void setValue(uint8_t param, double value) override;
  double getValue(uint8_t param) const override {
    assert(param < numberOfDoubleValues());
    return m_values[param];
  }

private:
  double m_values[k_numberOfDoubleValues];
};

class Data {
public:
  // By default, select the simple interest data model
  Data() : m_selectedModel(true) {}
  void setModel(bool selectedModel) { m_selectedModel = selectedModel; }
  InterestData * interestData() {
    return m_selectedModel ? static_cast<InterestData *>(&m_simpleInterestData) : static_cast<InterestData *>(&m_compoundInterestData);
  }

private:
  CompoundInterestData m_compoundInterestData;
  SimpleInterestData m_simpleInterestData;
  bool m_selectedModel;
};

}  // namespace Finance

#endif /* FINANCE_DATA_H */
