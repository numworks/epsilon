#ifndef INFERENCE_MODELS_STATISTIC_TWO_MEANS_STATISTIC_H
#define INFERENCE_MODELS_STATISTIC_TWO_MEANS_STATISTIC_H

#include "interfaces/significance_tests.h"
#include "statistic.h"

namespace Inference {

class TwoMeansStatistic {
 protected:
  TwoMeans::Type twoMeansType(const Statistic* stat) const {
    switch (stat->distributionType()) {
      case DistributionType::T:
        return TwoMeans::Type::T;
      case DistributionType::TPooled:
        return TwoMeans::Type::TPooled;
      default:
        assert(stat->distributionType() == DistributionType::Z);
        return TwoMeans::Type::Z;
    }
  }

  double m_params[TwoMeans::k_numberOfParams];
};

}  // namespace Inference

#endif
