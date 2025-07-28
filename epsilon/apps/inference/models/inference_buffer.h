#ifndef INFERENCE_MODELS_BUFFER_H
#define INFERENCE_MODELS_BUFFER_H

#include <new>

#include "goodness_test.h"
#include "homogeneity_test.h"
#include "one_mean_interval.h"
#include "one_mean_test.h"
#include "one_proportion_statistic.h"
#include "slope_t_statistic.h"
#include "two_means_interval.h"
#include "two_means_test.h"
#include "two_proportions_statistic.h"

namespace Inference {

// Buffers for dynamic allocation

union InferenceBuffer {
 public:
  InferenceBuffer() {
    new (&m_oneMeanTInterval) OneMeanTInterval(nullptr);
    inference()->initParameters();
  }
  ~InferenceBuffer() { inference()->~InferenceModel(); }
  // Rule of 5
  InferenceBuffer(const InferenceBuffer& other) = delete;
  InferenceBuffer(InferenceBuffer&& other) = delete;
  InferenceBuffer& operator=(const InferenceBuffer& other) = delete;
  InferenceBuffer& operator=(InferenceBuffer&& other) = delete;

  InferenceModel* inference() {
    return reinterpret_cast<InferenceModel*>(this);
  }

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

}  // namespace Inference

#endif
