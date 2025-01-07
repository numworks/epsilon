#ifndef INFERENCE_MODELS_STATISTIC_ONE_MEAN_STATISTIC_H
#define INFERENCE_MODELS_STATISTIC_ONE_MEAN_STATISTIC_H

#include "raw_data_statistic.h"

namespace Inference {

class OneMeanStatistic : public RawDataStatistic {
 public:
  using RawDataStatistic::RawDataStatistic;

  int numberOfSeries() const override { return 1; }

  int series() const { return seriesAt(0); }
  void setSeries(int series, Statistic* stat) { setSeriesAt(stat, 0, series); }

 protected:
  OneMean::Type oneMeanType(const Statistic* s) const {
    if (s->distributionType() == DistributionType::T) {
      return OneMean::Type::T;
    }
    assert(s->distributionType() == DistributionType::Z);
    return OneMean::Type::Z;
  }

  void syncParametersWithStore(const Statistic* stat) override;

  double m_params[OneMean::k_numberOfParams];
};

}  // namespace Inference

#endif
