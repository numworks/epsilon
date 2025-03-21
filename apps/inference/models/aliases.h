#ifndef INFERENCE_MODELS_SUBAPP_H
#define INFERENCE_MODELS_SUBAPP_H

#include <poincare/statistics/inference.h>

namespace Inference {

enum class SubApp {
  SignificanceTest,
  ConfidenceInterval,
};

constexpr static int k_numberOfSubApps = 2;

using TestType = Poincare::Inference::TestType;
using StatisticType = Poincare::Inference::StatisticType;
using CategoricalType =
    Poincare::Inference::SignificanceTest::Chi2::CategoricalType;

namespace Params = Poincare::Inference::Params;
}  // namespace Inference

#endif
