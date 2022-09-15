#ifndef INFERENCE_MODELS_STATISTIC_BUFFER_H
#define INFERENCE_MODELS_STATISTIC_BUFFER_H

#include <new>

#include "statistic/goodness_test.h"
#include "statistic/homogeneity_test.h"
#include "statistic/hypothesis_params.h"
#include "statistic/one_mean_t_interval.h"
#include "statistic/one_mean_t_test.h"
#include "statistic/one_mean_z_interval.h"
#include "statistic/one_mean_z_test.h"
#include "statistic/one_proportion_z_interval.h"
#include "statistic/one_proportion_z_test.h"
#include "statistic/pooled_two_means_t_interval.h"
#include "statistic/pooled_two_means_t_test.h"
#include "statistic/two_means_t_interval.h"
#include "statistic/two_means_t_test.h"
#include "statistic/two_means_z_interval.h"
#include "statistic/two_means_z_test.h"
#include "statistic/two_proportions_z_interval.h"
#include "statistic/two_proportions_z_test.h"

namespace Inference {

// Buffers for dynamic allocation

union StatisticBuffer {
public:
  StatisticBuffer() {
    new (&m_oneMeanTInterval) OneMeanTInterval();
    statistic()->initParameters();
  }
  ~StatisticBuffer() { statistic()->~Statistic(); }
  // Rule of 5
  StatisticBuffer(const StatisticBuffer& other) = delete;
  StatisticBuffer(StatisticBuffer&& other) = delete;
  StatisticBuffer& operator=(const StatisticBuffer& other) = delete;
  StatisticBuffer& operator=(StatisticBuffer&& other) = delete;

  Statistic * statistic() { return reinterpret_cast<Statistic *>(this); }
private:
  OneMeanTInterval m_oneMeanTInterval;
  OneMeanTTest m_oneMeanTTest;
  OneMeanZInterval m_oneMeanZInterval;
  OneMeanZTest m_oneMeanZTest;
  OneProportionZInterval m_oneProportionZInterval;
  OneProportionZTest m_oneProportionZTest;
  PooledTwoMeansTInterval m_pooledTwoMeansTInterval;
  PooledTwoMeansTTest m_pooledTwoMeansTTest;
  TwoMeansTInterval m_twoMeansTInterval;
  TwoMeansTTest m_twoMeansTTest;
  TwoMeansZInterval m_twoMeansZInterval;
  TwoMeansZTest m_twoMeansZTest;
  TwoProportionsZInterval m_twoProportionsZInterval;
  TwoProportionsZTest m_twoProportionsZTest;
  GoodnessTest m_goodnessTest;
  HomogeneityTest m_homogeneityTest;
};

}  // namespace Inference

#endif /* INFERENCE_MODELS_BUFFER_H */
