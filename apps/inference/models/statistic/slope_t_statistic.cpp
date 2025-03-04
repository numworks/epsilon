#include "slope_t_statistic.h"

#include <omg/round.h>

#include "poincare/src/statistics/inference/inference.h"

namespace Inference {

void SlopeTStatistic::computeParametersFromSeries(const Statistic* stat,
                                                  int pageIndex) {
  assert(hasSeries(pageIndex));
  int series = seriesAt(pageIndex);
  double n = doubleCastedNumberOfPairsOfSeries(series);
  double xMean = meanOfColumn(series, 0);
  double SE = std::sqrt((1.0 / (n - 2.0)) * leastSquaredSum(series) /
                        squaredOffsettedValueSumOfColumn(series, 0, xMean));

  m_params[Poincare::Inference::Params::Slope::N] = n;
  m_params[Poincare::Inference::Params::Slope::SE] = OMG::LaxToZero(SE);
  m_params[Poincare::Inference::Params::Slope::B] = slope(series);
}

void SlopeTTest::extraResultAtIndex(int index, double* value,
                                    Poincare::Layout* message,
                                    I18n::Message* subMessage, int* precision) {
  enum ResultOrder { B, SE };
  if (index == ResultOrder::B) {
    *value = m_params[Poincare::Inference::Params::Slope::B];
    *message =
        PcrInference::ParameterLayout(type(), PcrInference::Params::Slope::B);
    *subMessage = I18n::Message::SampleSlope;
    return;
  }
  assert(index == ResultOrder::SE);
  *value = m_params[Poincare::Inference::Params::Slope::SE];
  *message =
      PcrInference::ParameterLayout(type(), PcrInference::Params::Slope::SE);
  *subMessage = I18n::Message::StandardError;
}

void SlopeTInterval::extraResultAtIndex(int index, double* value,
                                        Poincare::Layout* message,
                                        I18n::Message* subMessage,
                                        int* precision) {
  assert(index == 0);
  *value = m_params[Poincare::Inference::Params::Slope::B];
  *message =
      PcrInference::ParameterLayout(type(), PcrInference::Params::Slope::B);
  *subMessage = I18n::Message::SampleSlope;
}

}  // namespace Inference
