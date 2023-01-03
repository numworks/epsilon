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
#include "statistic/slope_t_interval.h"
#include "statistic/slope_t_test.h"
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
  // Tests
  OneMeanTTest m_oneMeanTTest;
  OneMeanZTest m_oneMeanZTest;
  OneProportionZTest m_oneProportionZTest;
  PooledTwoMeansTTest m_pooledTwoMeansTTest;
  TwoMeansTTest m_twoMeansTTest;
  TwoMeansZTest m_twoMeansZTest;
  TwoProportionsZTest m_twoProportionsZTest;
  GoodnessTest m_goodnessTest;
  HomogeneityTest m_homogeneityTest;
  SlopeTTest m_slopeTTest;

  // Intervals;
  OneMeanTInterval m_oneMeanTInterval;
  OneMeanZInterval m_oneMeanZInterval;
  OneProportionZInterval m_oneProportionZInterval;
  PooledTwoMeansTInterval m_pooledTwoMeansTInterval;
  TwoMeansTInterval m_twoMeansTInterval;
  TwoMeansZInterval m_twoMeansZInterval;
  TwoProportionsZInterval m_twoProportionsZInterval;
  SlopeTInterval m_slopeTInterval;
};

}

#endif
