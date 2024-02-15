#ifndef INFERENCE_MODELS_STATISTIC_ONE_MEAN_STATISTIC_H
#define INFERENCE_MODELS_STATISTIC_ONE_MEAN_STATISTIC_H

#include "test.h"

namespace Inference {

class OneMeanStatistic {
 protected:
  static OneMean::Type OneMeanType(const Statistic* s) {
    if (s->distributionType() == DistributionType::T) {
      return OneMean::Type::T;
    }
    assert(s->distributionType() == DistributionType::Z);
    return OneMean::Type::Z;
  }

  double m_params[OneMean::k_numberOfParams];
};

}  // namespace Inference

#endif
