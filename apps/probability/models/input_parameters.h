#ifndef APPS_PROBABILITY_MODELS_DISTRIBUTION_INPUT_PARAMETERS_H
#define APPS_PROBABILITY_MODELS_DISTRIBUTION_INPUT_PARAMETERS_H

#include <apps/i18n.h>

#include "probability/helpers.h"

namespace Probability {

struct ParameterRepr {
  I18n::Message m_symbol;
  I18n::Message m_description;
};

class InputParameters {
public:
  virtual int numberOfParameters() const = 0;
  float paramAtIndex(int i) { return paramArray()[i]; }
  void setParamAtIndex(int i, float p) { paramArray()[i] = p; }
  I18n::Message paramSymbolAtIndex(int i) const { return paramReprAtIndex(i)->m_symbol; }
  I18n::Message paramDescriptionAtIndex(int i) const { return paramReprAtIndex(i)->m_description; }
  float significanceLevel() const { return m_significanceLevel; }
  void setSignificanceLevel(float s) { m_significanceLevel = s; }

protected:
  virtual const ParameterRepr * paramReprAtIndex(int i) const = 0;
  virtual float * paramArray() = 0;

  float m_significanceLevel;
};

class ZTestOnePropInputParameters : public InputParameters {
public:
  int numberOfParameters() const override { return k_numberOfParams; }

private:
  constexpr static int k_numberOfParams = 2;
  const ParameterRepr * paramReprAtIndex(int i) const override {
    constexpr static ParameterRepr params[k_numberOfParams] = {
        {I18n::Message::X, I18n::Message::NumberOfSuccesses},
        {I18n::Message::N, I18n::Message::SampleSize}};
    return &(params[i]);
  }
  float * paramArray() override { return m_params; }

  float m_params[k_numberOfParams];
};

class TTestOneMeanInputParameters : public InputParameters {
public:
  int numberOfParameters() const override { return k_numberOfParams; }

private:
  constexpr static int k_numberOfParams = 3;
  const ParameterRepr * paramReprAtIndex(int i) const override {
    constexpr static ParameterRepr params[k_numberOfParams] = {
        {I18n::Message::MeanSymbol, I18n::Message::SampleMean},
        {I18n::Message::s, I18n::Message::StandardDeviation},
        {I18n::Message::N, I18n::Message::SampleSize}};
    return &(params[i]);
  }
  float * paramArray() override { return m_params; }

  float m_params[k_numberOfParams];
};

constexpr static int classesSizes[] = {sizeof(ZTestOnePropInputParameters),
                                       sizeof(TTestOneMeanInputParameters)};
constexpr static int maxClassesSize = arrayMax(classesSizes);

typedef char InputParametersBuffer[maxClassesSize];

}  // namespace Probability

#endif /* APPS_PROBABILITY_MODELS_DISTRIBUTION_INPUT_PARAMETERS_H */
