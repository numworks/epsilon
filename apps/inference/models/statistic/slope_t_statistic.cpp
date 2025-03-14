#include "slope_t_statistic.h"

#include <apps/shared/store_to_series.h>
#include <omg/round.h>

#include "inference/models/statistic/aliases.h"
#include "poincare/src/statistics/inference/inference.h"

namespace Inference {

void SlopeTStatistic::computeParametersFromSeries(const Statistic* stat,
                                                  int pageIndex) {
  assert(hasSeries(pageIndex));
  int seriesIndex = seriesAt(pageIndex);
  Shared::StoreToSeries seriesModel(this, seriesIndex);
  Poincare::Inference::ParametersArray array =
      Poincare::Inference::ComputeSlopeParametersFromSeries(seriesModel);
  constexpr int nParams =
      Poincare::Inference::NumberOfParameters(TestType::Slope);
  for (int i = 0; i < nParams; i++) {
    m_params[i] = array[i];
  }
}

void SlopeTTest::extraResultAtIndex(int index, double* value,
                                    Poincare::Layout* message,
                                    I18n::Message* subMessage, int* precision) {
  enum ResultOrder { B, SE };
  if (index == ResultOrder::B) {
    *value = m_params[Params::Slope::B];
    *message = Poincare::Inference::ParameterLayout(type(), Params::Slope::B);
    *subMessage = I18n::Message::SampleSlope;
    return;
  }
  assert(index == ResultOrder::SE);
  *value = m_params[Params::Slope::SE];
  *message = Poincare::Inference::ParameterLayout(type(), Params::Slope::SE);
  *subMessage = I18n::Message::StandardError;
}

void SlopeTInterval::extraResultAtIndex(int index, double* value,
                                        Poincare::Layout* message,
                                        I18n::Message* subMessage,
                                        int* precision) {
  assert(index == 0);
  *value = m_params[Params::Slope::B];
  *message = Poincare::Inference::ParameterLayout(type(), Params::Slope::B);
  *subMessage = I18n::Message::SampleSlope;
}

}  // namespace Inference
